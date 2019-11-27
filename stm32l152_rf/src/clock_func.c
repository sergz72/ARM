#include <board.h>
#include <st7066.h>
#include <clock_func.h>
#include <rtc_func.h>

static rtc_data rtc;
static unsigned int unix_time, update_hours, update_minutes, update_others;
static unsigned int counter;

static const char *weekdays[7] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
static const char *months[12]  = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void ClockInit(void)
{
  unix_time = 0;
  counter = SENSOR_UPDATE_PERIOD;
  rtc_from_binary(unix_time, &rtc);
  update_hours = update_minutes = update_others = 1;
}

void ClockUpdate(unsigned int time)
{
  unix_time = time;
  rtc_from_binary(unix_time, &rtc);
  update_hours = update_minutes = update_others = 1;
}

unsigned int ClockLoop(void)
{
  unsigned int v;
  unsigned char *p;

  unix_time++;
  counter++;

  if (update_others)
  {
    st7066_set_ddram_address(0);
    p = weekdays[rtc.wday];
    st7066_write_data(*p++);
    st7066_write_data(*p);
    st7066_write_data(' ');
    p = months[rtc.month];
    st7066_write_data(*p++);
    st7066_write_data(*p++);
    st7066_write_data(*p);
    st7066_write_data(' ');
    v = rtc.day / 10;
    st7066_write_data(v ? '0' + v : ' ');
    st7066_write_data('0' + (rtc.day % 10));
    st7066_write_data(' ');
    st7066_write_data(rtc.year / 1000 + '0');
    st7066_write_data(((rtc.year / 100) % 10) + '0');
    st7066_write_data(((rtc.year / 10) % 10) + '0');
    st7066_write_data((rtc.year % 10) + '0');
    st7066_write_data(' ');
  }
  else
  {
    if (rtc.seconds == 59)
    {
      rtc.seconds = 0;
      if (rtc.minutes == 59)
      {
        rtc.minutes = 0;
        if (rtc.hours == 23)
          rtc.hours = 0;
        else
          rtc.hours++;
        update_hours = 1;
      } else
        rtc.minutes++;
      update_minutes = 1;
    } else
      rtc.seconds++;
  }

  if (update_hours)
  {
    st7066_set_ddram_address(15);
    v = rtc.hours / 10;
    if (!v)
      st7066_write_data(' ');
    else
      st7066_write_data('0' + v);
    v = rtc.hours % 10;
    st7066_write_data('0' + v);
  }
  else
    st7066_set_ddram_address(17);
  st7066_write_data(rtc.seconds & 1U ? ':' : ' ');
  if (update_minutes)
  {
    v = rtc.minutes / 10;
    st7066_write_data('0' + v);
    v = rtc.minutes % 10;
    st7066_write_data('0' + v);
  }

  update_hours = update_minutes = update_others = 0;

  if (counter >= SENSOR_UPDATE_PERIOD)
  {
    counter = 0;
    return 1;
  }
  return 0;
}
