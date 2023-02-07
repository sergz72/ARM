#include <board.h>
#include <systick.h>

#define  SysTick_CTRL_ENABLE                 0x00000001U        /*!< Counter enable */
#define  SysTick_CTRL_TICKINT                0x00000002U        /*!< Counting down to 0 pends the SysTick handler */
#define  SysTick_CTRL_COUNTFLAG              0x00010000U        /*!< Count Flag */

void systick_set_reload(unsigned int value)
{
  SysTick->LOAD = (value & SYSTICK_MAX_VALUE);
}

void systick_set_value(unsigned int value)
{
  SysTick->VAL = (value & SYSTICK_MAX_VALUE);
}

unsigned int systick_get_value(void)
{
  return SysTick->VAL;
}

unsigned int systick_get_us(unsigned int start)
{
  unsigned int delta = start - systick_get_value();
  return delta / SYSTICK_MULTIPLIER;
}

void systick_set_clocksource(unsigned int clocksource)
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

void systick_start_noint(void)
{
  systick_set_reload(SYSTICK_MAX_VALUE);
  systick_set_value(0);

  /* start counting */
  systick_counter_enable();
}
