#ifndef _RTC_DS1307_H
#define _RTC_DS1307_H

#include <rtc_func.h>

#define DS1307_I2C_ADDRESS 0xD0

int ds1307_init(int enable_sqw);
int ds1307_get(rtc_data *data);
int ds1307_set_datetime(int year, int month, int day, int hour, int minute, int seconds);

int i2c_ds1307_write(const unsigned char *data, int data_length); // should be defined in hal.c
int i2c_ds1307_transfer(const unsigned char *wdata, int wdata_length, unsigned char *rdata, int rdata_length); // should be defined in hal.c

#endif
