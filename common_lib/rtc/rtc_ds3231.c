#include <board.h>
#include <rtc_ds3231.h>

#define ADDRRTC 0xD0

void rtc_init(int int_freq)
{
  unsigned char data[3];
  
  data[0] = 0x0E;
  if (int_freq == 1)
    data[1] = 0;
  data[2] = 0;

  /* write register address, control register */
  /* enable sqw output */
  /* disable 32 khz output output */
  i2c_ds3231_write(ADDRRTC, data, 3, 1);
}

unsigned char pack(unsigned char data)
{
  return (data & 0xF) + (data >> 4) * 10;
}

unsigned char unpack(unsigned char data)
{
  return (data % 10) + ((data / 10) << 4);
}

void rtc_get(rtc_data *data)
{
  unsigned char data2[7];

  data2[0] = 0;
  i2c_ds3231_write(ADDRRTC, data2, 1, 0);
  i2c_ds3231_read(ADDRRTC|1, data2, 7);
  data->seconds = pack(data2[0]);
  data->minutes = pack(data2[1]);
  data->hours   = pack(data2[2]);
  data->wday    = data2[3];
  data->day     = pack(data2[4]);
  data->month   = pack(data2[5]);
  data->year    = 2000 + pack(data2[6]);
  data->yday    = rtc_get_yday(data->year, data->month, data->day);
}

float rtc_get_temp(void)
{
  union {
    unsigned char data[2];
    short temp;
  } d;
  float temp;
  unsigned char c;

  do
  {
    d.data[0] = 0x0E;
    i2c_ds3231_write(ADDRRTC, d.data, 1, 0);
    i2c_ds3231_read(ADDRRTC|1, d.data, 1);
  } while (d.data[0] & 0x20);

  d.data[0] = 0x11;
  i2c_ds3231_write(ADDRRTC, d.data, 1, 0);
  i2c_ds3231_read(ADDRRTC|1, d.data, 2);
  c = d.data[0];
  d.data[0] = d.data[1];
  d.data[1] = c;
  temp = d.temp >> 6;

  return temp / 4;
}

void rtc_set_date(int year, int month, int day)
{
  unsigned char data[5];
  
  data[0] = 3;
  data[1] = rtc_get_wday(year, month, day);
  data[2] = unpack(day);
  data[3] = unpack(month);
  data[4] = unpack(year - 2000);

  i2c_ds3231_write(ADDRRTC, data, 5, 1);
}

void rtc_set_time(int hour, int minute)
{
  unsigned char data[4];
  
  data[0] = data[1] = 0;
  data[2] = unpack(minute);
  data[3] = unpack(hour);

  i2c_ds3231_write(ADDRRTC, data, 4, 1);
}
