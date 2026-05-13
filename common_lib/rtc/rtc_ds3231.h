#ifndef _RTC_DS3231_H
#define _RTC_DS3231_H

#include <rtc_func.h>

#define DS3231_I2C_ADDRESS 0xD0

int ds3231_init(int enable_sqw);
int ds3231_get(rtc_data *data);
int ds3231_set_datetime(int year, int month, int day, int hour, int minute, int seconds);
int ds3231_get_temp(short *result);

int i2c_ds3231_write(const unsigned char *data, int data_length); // should be defined in hal.c
int i2c_ds3231_transfer(const unsigned char *wdata, int wdata_length, unsigned char *rdata, int rdata_length); // should be defined in hal.c

#endif
