#include "date_commands.h"
#include <shell.h>
#include <rtc.h>
#include <stdlib.h>

static int date_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem date_get_command_items[] = {
  {nullptr, nullptr, date_get_handler}
};
static const ShellCommand date_get_command = {
  date_get_command_items,
  "date_get",
  "date_get",
  nullptr,
  nullptr
};

static int date_set_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem date_set_command_items[] = {
  {nullptr, param_handler, nullptr}, // year
  {nullptr, param_handler, nullptr}, // month
  {nullptr, param_handler, nullptr}, // day
  {nullptr, param_handler, nullptr}, // hour
  {nullptr, param_handler, nullptr}, // minute
  {nullptr, param_handler, nullptr}, // second
  {nullptr, nullptr, date_set_handler}
};
static const ShellCommand date_set_command = {
  date_set_command_items,
  "date_set",
  "date_set year month day hour minute second",
  nullptr,
  nullptr
};

static int date_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  rtc_data rdata;
  RTC_GetDateTime(&rdata);
  pfunc("Date: %d.%02d.%04d %02d:%02d:%02d weekday=%d yday=%d\n", rdata.day, rdata.month, rdata.year, rdata.hours,
        rdata.minutes, rdata.seconds, rdata.wday, rdata.yday);
  return 0;
}

static int date_set_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int year = atoi(argv[0]);
  int month = atoi(argv[1]);
  int day = atoi(argv[2]);
  int hour = atoi(argv[3]);
  int minute = atoi(argv[4]);
  int second = atoi(argv[5]);
  if (year < 2026)
  {
    pfunc("Invalid year\r\n");
    return 1;
  }
  if (month < 1 || month > 12)
  {
    pfunc("Invalid month\r\n");
    return 1;
  }
  if (day < 1 || day > 31)
  {
    pfunc("Invalid day\r\n");
    return 1;
  }
  if (hour < 0 || hour > 23)
  {
    pfunc("Invalid hour\r\n");
    return 1;
  }
  if (minute < 0 || minute > 59)
  {
    pfunc("Invalid minute\r\n");
    return 1;
  }
  if (second < 0 || second > 59)
  {
    pfunc("Invalid second\r\n");
    return 1;
  }
  rtc_data rdata;
  rdata.year = year;
  rdata.month = month;
  rdata.day = day;
  rdata.hours = hour;
  rdata.minutes = minute;
  rdata.seconds = second;
  return RTC_InitDateTime(&rdata);
}

void register_date_commands(void)
{
  shell_register_command(&date_get_command);
  shell_register_command(&date_set_command);
}
