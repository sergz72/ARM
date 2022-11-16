#include <rtc_func.h>

// this array represents the number of days in one non-leap year at
// the beginning of each month
unsigned int DaysToMonth[13] = {
  0,31,59,90,120,151,181,212,243,273,304,334,365
};

unsigned int rtc_to_binary(rtc_data *data)
{
  unsigned int iday;
  unsigned int val;
  
  iday = 365 * (data->year - 1970) + DaysToMonth[data->month - 1] + (data->day - 1);
  iday += (data->year - 1969) / 4;
  if ((data->month > 2) && ((data->year % 4) == 0))
    iday++;
  
  val = data->seconds + 60 * data->minutes + 3600 * (data->hours + 24 * iday);

  return val;
}

void rtc_from_binary(unsigned int binary, rtc_data *data)
{
  unsigned int hour;
  unsigned int day;
  unsigned int minute;
  unsigned int second;
  unsigned int month;
  unsigned int year;
  unsigned int whole_minutes;
  unsigned int whole_hours;
  unsigned int whole_days;
  unsigned int whole_days_since_1968;
  unsigned int leap_year_periods;
  unsigned int days_since_current_lyear;
  unsigned int whole_years;
  unsigned int days_since_first_of_year;
  unsigned int days_to_month;
  unsigned int day_of_week;

  whole_minutes = binary / 60;
  second = binary - (60 * whole_minutes); // leftover seconds
  whole_hours = whole_minutes / 60;
  minute = whole_minutes - (60 * whole_hours); // leftover minutes
  whole_days = whole_hours / 24;
  hour = whole_hours - (24 * whole_days); // leftover hours
  whole_days_since_1968 = whole_days + 365 + 366;
  leap_year_periods = whole_days_since_1968 / ((4 * 365) + 1);
  days_since_current_lyear = whole_days_since_1968 % ((4 * 365) + 1);
  // if days are after a current leap year then add a leap year period
  if ((days_since_current_lyear >= (31 + 29)))
    leap_year_periods++;
  whole_years = (whole_days_since_1968 - leap_year_periods) / 365;
  days_since_first_of_year = whole_days_since_1968 - (whole_years * 365) - leap_year_periods;
  if ((days_since_current_lyear <= 365) && (days_since_current_lyear >= 60))
    days_since_first_of_year++;
  year = whole_years + 68;
// setup for a search for what month it is based on how many days have past
// within the current year
  month = 13;
  days_to_month = 366;
  while (days_since_first_of_year < days_to_month)
  {
    month--;
    days_to_month = DaysToMonth[month-1];
    if ((month > 2) && ((year % 4) == 0))
      days_to_month++;
  }
  day = days_since_first_of_year - days_to_month + 1;
  day_of_week = (whole_days + 4) % 7;

  data->yday    = days_since_first_of_year; /* days since January 1 - [0,365] */
  data->seconds = second; /* seconds after the minute - [0,59] */
  data->minutes = minute; /* minutes after the hour - [0,59] */
  data->hours   = hour; /* hours since midnight - [0,23] */
  data->day     = day; /* day of the month - [1,31] */
  data->wday    = day_of_week; /* days since Sunday - [0,6] */
  data->month   = month - 1; /* months since January - [0,11] */
  data->year    = year + 1900; /* years */
}

unsigned int rtc_get_wday(unsigned int year, unsigned int month, unsigned int day)
{
  rtc_data data;

  data.year = year;
  data.month = month;
  data.day = day;
  data.hours = data.minutes = data.seconds = 0;
  rtc_from_binary(rtc_to_binary(&data), &data);
  return data.wday;
}

unsigned int rtc_get_yday(unsigned int year, unsigned int month, unsigned int day)
{
  rtc_data data;

  data.year = year;
  data.month = month;
  data.day = day;
  data.hours = data.minutes = data.seconds = 0;
  rtc_from_binary(rtc_to_binary(&data), &data);
  return data.yday;
}
