#include <board.h>
#include <rtc_ds1307.h>

int ds1307_init(int enable_sqw)
{
  unsigned char data[2];

  data[0] = 0x07;
  data[1] = enable_sqw ? 0x10 : 0;

  /* write register address, control register */
  /* enable sqw output */
  return i2c_ds1307_write(data, 2);
}

static unsigned char pack(unsigned char data)
{
  return (data & 0xF) + (data >> 4) * 10;
}

static unsigned char unpack(unsigned char data)
{
  return (data % 10) + ((data / 10) << 4);
}

int ds1307_get(rtc_data *data)
{
  unsigned char data2[7];

  data2[0] = 0;
  int rc = i2c_ds1307_transfer(data2, 1, data2, 7);
  if (rc != 0)
    return rc;
  data->seconds = pack(data2[0]);
  data->minutes = pack(data2[1]);
  data->hours   = pack(data2[2]);
  data->wday    = data2[3];
  data->day     = pack(data2[4]);
  data->month   = pack(data2[5]);
  data->year    = 2000 + pack(data2[6]);
  data->yday    = rtc_get_yday(data->year, data->month, data->day);
  return 0;
}

int ds1307_set_datetime(int year, int month, int day, int hour, int minute, int seconds)
{
  unsigned char data[8];

  data[0] = 0;
  data[1] = unpack(seconds);
  data[2] = unpack(minute);
  data[3] = unpack(hour);
  data[4] = rtc_get_wday(year, month, day);
  data[5] = unpack(day);
  data[6] = unpack(month);
  data[7] = unpack(year - 2000);

  return i2c_ds1307_write(data, 8);
}
