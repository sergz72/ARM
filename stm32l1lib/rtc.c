#include <board.h>
#include <rtc.h>

void RTC_WakeUpConfig(unsigned int config, unsigned int seconds)
{
  /* Clear the Wakeup Timer clock source bits in CR register */
  RTC->CR &= (uint32_t)~(RTC_CR_WUCKSEL | RTC_CR_WUTE);

  while (!(RTC->ISR & RTC_ISR_WUTWF))
    ;

  /* Configure the clock source */
  RTC->CR |= config;

  /* Configure the Wakeup Timer counter */
  RTC->WUTR = seconds - 1;
  RTC->CR |= RTC_CR_WUTE | RTC_CR_WUTIE;

  // Enable EXIT line 20
  EXTI->RTSR = 1 << 20U;
  EXTI->IMR = 1 << 20U;
}

void RTC_InitTime(unsigned int hours, unsigned int minutes, unsigned int seconds)
{
  RTC->ISR = RTC_ISR_INIT;
  while (!(RTC->ISR & RTC_ISR_INITF))
    ;
  unsigned int tr = seconds % 10;
  tr |= (seconds / 10) << 4U;
  tr |= (minutes % 10) << 8U;
  tr |= (minutes / 10) << 12U;
  tr |= (hours % 10) << 16U;
  tr |= (hours / 10) << 20U;
  RTC->TR = tr;

  RTC->ISR = 0;
}