#include "board.h"
#include <rtc.h>

static void RTCInitEnter(void)
{
  // Disable RTC write protection
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  // Enter RTC Initialization Mode
  RTC->ICSR = RTC_ICSR_INIT;
  while (!(RTC->ICSR & RTC_ICSR_INITF))
    ;
}

static void RTCInitLeave(void)
{
  // Exit RTC Initialization Mode
  RTC->ICSR &= ~RTC_ICSR_INIT;

  // Re-enable write protection
  RTC->WPR = 0xFF;
}

void RTCInit(unsigned int lsedrv)
{
  // Disable Backup Domain write protection
  PWR->DBPR = PWR_DBPR_DBP;

  // Enable LSE and wait for it to be ready
  RCC->BDCR |= RCC_BDCR_LSEON | lsedrv;
  while (!(RCC->BDCR & RCC_BDCR_LSERDY))
    ;

  // Select LSE as the RTC clock source (01: LSE oscillator clock used as RTC clock)
  RCC->BDCR |= RCC_BDCR_RTCSEL_0;

  // Enable the RTC peripheral clock
  RCC->BDCR |= RCC_BDCR_RTCEN;
  RCC->APB1ENR1 |= RCC_APB1ENR1_RTCAPBEN;

  RTCInitEnter();

  // Set BCD Only Mode (BIN = 00)

  // Set Prescalers (e.g., LSE at 32.768kHz, PREDIV_S=256, PREVDIV_A = 128 for 1Hz)
  RTC->PRER = 0x007F00FF;

  RTCInitLeave();
}

void RTC_WakeUpConfig(unsigned int seconds)
{
  // 3. Disable RTC write protection
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  /* Clear the Wakeup Timer clock source bits in CR register */
  RTC->CR &= ~(RTC_CR_WUCKSEL | RTC_CR_WUTE);

  while (!(RTC->MISR & RTC_MISR_WUTMF))
    ;

  /* Configure the Wakeup Timer counter */
  RTC->WUTR = seconds - 1;
  RTC->CR |= RTC_CR_WUTE | RTC_CR_WUTIE;

  // 8. Re-enable write protection
  RTC->WPR = 0xFF;
}

int RTC_InitTime(unsigned int hours, unsigned int minutes, unsigned int seconds, bool init_enter, bool init_leave)
{
  if (hours > 23 || minutes > 59 || seconds > 59)
  {
    if (!init_enter && init_leave)
      RTCInitLeave();
    return 1;
  }

  if (init_enter)
    RTCInitEnter();

  unsigned int tr = seconds % 10;
  tr |= (seconds / 10) << 4U;
  tr |= (minutes % 10) << 8U;
  tr |= (minutes / 10) << 12U;
  tr |= (hours % 10) << 16U;
  tr |= (hours / 10) << 20U;
  RTC->TR = tr;

  if (init_leave)
    RTCInitLeave();

  return 0;
}

int RTC_InitDate(unsigned int year, unsigned int month, unsigned int day, bool init_enter, bool init_leave)
{
  if (year < 2026 || month == 0 || month > 12 || day == 0 || day > 31)
  {
    if (!init_enter && init_leave)
      RTCInitLeave();
    return 1;
  }

  unsigned char wday = rtc_get_wday(year, month, day) + 1;

  if (init_enter)
    RTCInitEnter();

  unsigned int dr = day % 10;
  dr |= (day / 10) << 4U;
  dr |= (month % 10) << 8U;
  dr |= (month / 10) << 12U;
  dr |= wday << 13U;
  dr |= (year % 10) << 16U;
  dr |= ((year / 10) % 10) << 20U;
  RTC->DR = dr;

  if (init_leave)
    RTCInitLeave();

  return 0;
}

int RTC_InitDateTime(const rtc_data *data)
{
  int rc = RTC_InitDate(data->year, data->month, data->day, true, false);
  if (rc)
    return rc;

  return RTC_InitTime(data->hours, data->minutes, data->seconds, false, true);
}

static unsigned char pack(unsigned int data)
{
  return (data & 0xF) + (data >> 4) * 10;
}

void RTC_GetDateTime(rtc_data *data)
{
  unsigned int tr = RTC->TR;
  unsigned int dr = RTC->DR;

  data->seconds = pack(tr & 0x7F);
  data->minutes = pack((tr >> 8) & 0x7F);
  data->hours   = pack((tr >> 16) & 0x3F);
  data->day     = pack(dr & 0x3F);
  data->wday    = (dr >> 13) & 7 - 1;
  data->month   = pack((dr >> 8) & 0x1F);
  data->year    = 2000 + pack((dr >> 16) & 0xFF);
  data->yday    = rtc_get_yday(data->year, data->month, data->day);
}
