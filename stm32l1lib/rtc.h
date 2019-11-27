#ifndef _RTC_H
#define _RTC_H

#define _RTC_WRITEPROTECTION_DISABLE            \
                        do{                     \
                            RTC->WPR = 0xCAU;   \
                            RTC->WPR = 0x53U;   \
                          } while(0U)

#define _RTC_WRITEPROTECTION_ENABLE             \
                        do{                     \
                            RTC->WPR = 0xFFU;   \
                          } while(0U)

void RTC_WakeUpConfig(unsigned int config, unsigned int seconds);
void RTC_InitTime(unsigned int hours, unsigned int minutes, unsigned int seconds);

#endif
