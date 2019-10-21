#include <board.h>
#include <systick.h>

#define  SysTick_CTRL_ENABLE                 ((uint32_t)0x00000001)        /*!< Counter enable */
#define  SysTick_CTRL_TICKINT                ((uint32_t)0x00000002)        /*!< Counting down to 0 pends the SysTick handler */
#define  SysTick_CTRL_COUNTFLAG              ((uint32_t)0x00010000)        /*!< Count Flag */

void systick_set_reload(unsigned int value)
{
  SysTick->LOAD = (value &  0x00FFFFFF);
}

void systick_set_value(unsigned int value)
{
  SysTick->VAL = (value &  0x00FFFFFF);
}

unsigned int systick_get_value(void)
{
  return SysTick->VAL;
}

void systick_set_clocksource(int clocksource)
{
  if (clocksource < 2)
    SysTick->CTRL |= (clocksource << STK_CTRL_CLKSOURCE_LSB);
}

void systick_interrupt_enable(void)
{
  SysTick->CTRL |= SysTick_CTRL_TICKINT;
}

void systick_interrupt_disable(void)
{
  SysTick->CTRL &= ~SysTick_CTRL_TICKINT;
}

void systick_counter_enable(void)
{
  SysTick->CTRL |= SysTick_CTRL_ENABLE;
}

void systick_counter_disable(void)
{
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
}

int systick_get_countflag(void)
{
  if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG)
    return 1;
  else
    return 0;	
}
