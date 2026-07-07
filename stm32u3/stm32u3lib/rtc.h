#ifndef _RTC_H
#define _RTC_H

#include <rtc_func.h>

void RTCInit(unsigned int lsedrv);
int RTC_InitTime(unsigned int hours, unsigned int minutes, unsigned int seconds, bool init_enter, bool init_leave);
int RTC_InitDate(unsigned int year, unsigned int month, unsigned int day, bool init_enter, bool init_leave);
int RTC_InitDateTime(const rtc_data *data);
void RTC_WakeUpConfig(unsigned int seconds);
void RTC_GetDateTime(rtc_data *data);

#endif
