#include <board.h>
#include <rtc_ds3231.h>

int ds3231_init(int enable_sqw)
{
  unsigned char data[3];
  
  data[0] = 0x0E;
  data[1] = enable_sqw ? 0 : 4;
  data[2] = 0;

  /* write register address, control register */
  /* enable sqw output */
  /* disable 32 khz output */
  return i2c_ds3231_write(data, 3);
}

static unsigned char pack(unsigned char data)
{
  return (data & 0xF) + (data >> 4) * 10;
}

static unsigned char unpack(unsigned char data)
{
  return (data % 10) + ((data / 10) << 4);
}

int ds3231_get(rtc_data *data)
{
  unsigned char data2[7];

  data2[0] = 0;
  int rc = i2c_ds3231_transfer(data2, 1, data2, 7);
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

int ds3231_get_temp(short *result)
{
  union {
    unsigned char data[2];
    short temp;
  } d;
  unsigned char c;
  int rc;

  do
  {
    d.data[0] = 0x0E;
    rc = i2c_ds3231_transfer(d.data, 1, d.data, 1);
    if (rc != 0)
      return rc;
  } while (d.data[0] & 0x20);

  d.data[0] = 0x11;
  rc = i2c_ds3231_transfer(d.data, 1, d.data, 2);
  if (rc != 0)
    return rc;
  c = d.data[0];
  d.data[0] = d.data[1];
  d.data[1] = c;

  *result = (d.temp >> 6) * 25;
  return 0;
}

int ds3231_set_datetime(int year, int month, int day, int hour, int minute, int seconds)
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

  return i2c_ds3231_write(data, 8);
}
