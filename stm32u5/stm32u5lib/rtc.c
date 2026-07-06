#include "board.h"
#include <rtc.h>

void RTCInit(void)
{
  // Disable Backup Domain write protection
  PWR->DBPR = PWR_DBPR_DBP;

  // Enable LSE and wait for it to be ready
  RCC->BDCR |= RCC_BDCR_LSEON | RCC_BDCR_LSEDRV_1 | RCC_BDCR_LSEDRV_0;
  while (!(RCC->BDCR & RCC_BDCR_LSERDY))
    ;

  // Select LSE as the RTC clock source (01: LSE oscillator clock used as RTC clock)
  RCC->BDCR |= RCC_BDCR_RTCSEL_0;

  // Enable the RTC peripheral clock
  RCC->BDCR |= RCC_BDCR_RTCEN;
  RCC->APB3ENR |= RCC_APB3ENR_RTCAPBEN;
  RCC->SRDAMR |= RCC_SRDAMR_RTCAPBAMEN;

  // Disable RTC write protection
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  // Enter RTC Initialization Mode
  RTC->ICSR = RTC_ICSR_INIT;
  while (!(RTC->ICSR & RTC_ICSR_INITF))
    ;

  RTC->SSR = 0xFFFFFFFF;

  // Set Binary Only Mode (BIN = 01)
  RTC->ICSR |= RTC_ICSR_BIN_0;

  // Set Prescalers (e.g., LSE at 32.768kHz, PREDIV_S=256, PREVDIV_A = 128 for 1Hz)
  RTC->PRER = 0x007F00FF;

  // Exit RTC Initialization Mode
  RTC->ICSR &= ~RTC_ICSR_INIT;

  // Re-enable write protection
  RTC->WPR = 0xFF;
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

void RTC_InitTime(unsigned int hours, unsigned int minutes, unsigned int seconds)
{
  // Disable RTC write protection
  RTC->WPR = 0xCA;
  RTC->WPR = 0x53;

  RTC->ICSR |= RTC_ICSR_INIT;
  while (!(RTC->ICSR & RTC_ICSR_INITF))
    ;
  unsigned int tr = seconds % 10;
  tr |= (seconds / 10) << 4U;
  tr |= (minutes % 10) << 8U;
  tr |= (minutes / 10) << 12U;
  tr |= (hours % 10) << 16U;
  tr |= (hours / 10) << 20U;
  RTC->TR = tr;

  RTC->ICSR &= ~RTC_ICSR_INIT;

  // 8. Re-enable write protection
  RTC->WPR = 0xFF;
}
