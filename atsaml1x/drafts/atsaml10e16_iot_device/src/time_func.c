#include "board.h"
#include "time_func.h"
#include "config.h"
#include "rtc_ds1307.h"
#include "rtc_ds3231.h"
#include "rtc_pcf8563.h"

#define TIME_OFFSET 1784707500

unsigned int timestamp;

int rtc_init(void)
{
  int rc;
  switch (device_configuration.rtc_type)
  {
    case RTC_TYPE_DS3231:
      rc = ds3231_init(1);
      break;
    case RTC_TYPE_DS1307:
      rc = ds1307_init(1);
      break;
    case RTC_TYPE_PCF8563:
      rc = pcf8563_init(PCF8563_CLKOUT_1Hz);
      break;
    default:
      return 1;
  }
  if (rc)
    return rc;
  rtc_data data;
  switch (device_configuration.rtc_type)
  {
    case RTC_TYPE_DS3231:
      rc = ds3231_get(&data);
      break;
    case RTC_TYPE_DS1307:
      rc = ds1307_get(&data);
      break;
    case RTC_TYPE_PCF8563:
      rc = pcf8563_get(&data);
      break;
    default:
      return 1;
  }
  if (rc)
    return rc;
  timestamp = rtc_to_binary(&data) - TIME_OFFSET;
  return 0;
}
