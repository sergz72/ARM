#include "pcf8563_commands.h"
#include <shell.h>
#include <rtc_pcf8563.h>
#include <stdlib.h>
#include <string.h>

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init_command_items[] = {
  {nullptr, param_handler, nullptr}, // clkout
  {nullptr, nullptr, init_handler}
};
static const ShellCommand init_command = {
  init_command_items,
  "pcf8563_init",
  "pcf8563_init clkout",
  nullptr,
  nullptr
};

static int status_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem status_command_items[] = {
  {nullptr, nullptr, status_handler}
};
static const ShellCommand status_command = {
  status_command_items,
  "pcf8563_status",
  "pcf8563_status",
  nullptr,
  nullptr
};

static int date_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem date_get_command_items[] = {
  {nullptr, nullptr, date_get_handler}
};
static const ShellCommand date_get_command = {
  date_get_command_items,
  "pcf8563_date_get",
  "pcf8563_date_get",
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
  "pcf8563_date_set",
  "pcf8563_date_set year month day hour minute second",
  nullptr,
  nullptr
};

static int alarm_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem alarm_get_command_items[] = {
  {nullptr, nullptr, alarm_get_handler}
};
static const ShellCommand alarm_get_command = {
  alarm_get_command_items,
  "pcf8563_alarm_get",
  "pcf8563_alarm_get",
  nullptr,
  nullptr
};

static int alarm_set_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem alarm_set_command_items[] = {
  {nullptr, param_handler, nullptr}, // wday
  {nullptr, param_handler, nullptr}, // day
  {nullptr, param_handler, nullptr}, // hour
  {nullptr, param_handler, nullptr}, // minute
  {nullptr, nullptr, alarm_set_handler}
};
static const ShellCommand alarm_set_command = {
  alarm_set_command_items,
  "pcf8563_alarm_set",
  "pcf8563_alarm_set wday day hour minute",
  nullptr,
  nullptr
};

static int alarm_cancel_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem alarm_cancel_command_items[] = {
  {nullptr, nullptr, alarm_cancel_handler}
};
static const ShellCommand alarm_cancel_command = {
  alarm_cancel_command_items,
  "pcf8563_alarm_cancel",
  "pcf8563_alarm_cancel",
  nullptr,
  nullptr
};

static int timer_set_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem timer_set_command_items[] = {
  {nullptr, param_handler, nullptr}, // value
  {nullptr, param_handler, nullptr}, // clock
  {nullptr, nullptr, timer_set_handler}
};
static const ShellCommand timer_set_command = {
  timer_set_command_items,
  "pcf8563_timer_set",
  "pcf8563_timer_set value clock",
  nullptr,
  nullptr
};

static int timer_cancel_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem timer_cancel_command_items[] = {
  {nullptr, nullptr, timer_cancel_handler}
};
static const ShellCommand timer_cancel_command = {
  timer_cancel_command_items,
  "pcf8563_timer_cancel",
  "pcf8563_timer_cancel",
  nullptr,
  nullptr
};

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  if (!strcmp(argv[0], "off"))
    return pcf8563_init(PCF8563_CLKOUT_DISABLE);
  if (!strcmp(argv[0], "32768"))
    return pcf8563_init(PCF8563_CLKOUT_32768Hz);
  if (!strcmp(argv[0], "1024"))
    return pcf8563_init(PCF8563_CLKOUT_1024Hz);
  if (!strcmp(argv[0], "32"))
    return pcf8563_init(PCF8563_CLKOUT_32Hz);
  if (!strcmp(argv[0], "1"))
    return pcf8563_init(PCF8563_CLKOUT_1Hz);
  pfunc("Invalid clkout\r\n");
  return 1;
}

static int status_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned char status;
  int rc = pcf8563_get_status(&status);
  if (rc != 0)
    return rc;
  pfunc("Status: %02X\r\n", status);
  return 0;
}

static int date_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  rtc_data rdata;
  int rc = pcf8563_get(&rdata);
  if (rc != 0)
    return rc;
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
  return pcf8563_set_datetime(year, month, day, hour, minute, second);
}

static int alarm_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pcf8563_alarm adata;
  int rc = pcf8563_get_alarm(&adata);
  if (rc != 0)
    return rc;
  pfunc("Alarm: weekday=%d(%d) day=%d(%d) hour=%02d(%d) minute=%02d(%d)\n", adata.wday.value, adata.wday.enabled,
        adata.day.value, adata.day.enabled, adata.hour.value, adata.hour.enabled, adata.minute.value, adata.minute.enabled);
  return 0;
}

static int alarm_set_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int wday = atoi(argv[0]);
  int day = atoi(argv[1]);
  int hour = atoi(argv[2]);
  int minute = atoi(argv[3]);
  if (wday < -1 || wday > 6)
  {
    pfunc("Invalid wday\r\n");
    return 1;
  }
  if (day != -1 && (day < 1 || day > 31))
  {
    pfunc("Invalid day\r\n");
    return 1;
  }
  if (hour < -1 || hour > 23)
  {
    pfunc("Invalid hour\r\n");
    return 1;
  }
  if (minute < -1 || minute > 59)
  {
    pfunc("Invalid minute\r\n");
    return 1;
  }
  pcf8563_alarm adata;
  adata.wday.enabled = wday != -1;
  adata.wday.value = wday;
  adata.day.enabled = day != -1;
  adata.day.value = day;
  adata.hour.enabled = hour != -1;
  adata.hour.value = hour;
  adata.minute.enabled = minute != -1;
  adata.minute.value = minute;
  return pcf8563_set_alarm(&adata);
}

static int alarm_cancel_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return pcf8563_cancel_alarm();
}

static int timer_set_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  if (value < 0 || value > 255)
  {
    pfunc("Invalid value\r\n");
    return 1;
  }
  if (!strcmp(argv[1], "4096"))
    return pcf8563_set_timer(value, PCF8563_TIMER_CLOCK_4096Hz);
  if (!strcmp(argv[1], "64"))
    return pcf8563_set_timer(value, PCF8563_TIMER_CLOCK_64Hz);
  if (!strcmp(argv[1], "1"))
    return pcf8563_set_timer(value, PCF8563_TIMER_CLOCK_1Hz);
  if (!strcmp(argv[1], "1m"))
    return pcf8563_set_timer(value, PCF8563_TIMER_CLOCK_1d60Hz);
  pfunc("Invalid timer clock\r\n");
  return 1;
}

static int timer_cancel_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return pcf8563_cancel_timer();
}

void register_pcf8563_commands(void)
{
  shell_register_command(&init_command);
  shell_register_command(&status_command);
  shell_register_command(&date_get_command);
  shell_register_command(&date_set_command);
  shell_register_command(&alarm_get_command);
  shell_register_command(&alarm_set_command);
  shell_register_command(&alarm_cancel_command);
  shell_register_command(&timer_set_command);
  shell_register_command(&timer_cancel_command);
}
