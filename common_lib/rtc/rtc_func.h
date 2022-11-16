#ifndef _RTC_FUNC_H
#define _RTC_FUNC_H

typedef struct {
  unsigned int month, day, hours, minutes, seconds, wday;
  unsigned int year, yday;
} rtc_data;

unsigned int rtc_to_binary(rtc_data *data);
void rtc_from_binary(unsigned int binary, rtc_data *data);
unsigned int rtc_get_wday(unsigned int year, unsigned int month, unsigned int day);
unsigned int rtc_get_yday(unsigned int year, unsigned int month, unsigned int day);

#endif
