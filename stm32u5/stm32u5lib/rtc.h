#ifndef _RTC_H
#define _RTC_H

void RTCInit(void);
void RTC_InitTime(unsigned int hours, unsigned int minutes, unsigned int seconds);
void RTC_WakeUpConfig(unsigned int seconds);

#endif
