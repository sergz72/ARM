#include "board.h"
#include <systick.h>
#include <eth.h>

volatile int Sys_Tick;

void SysTick_Handler(void)
{
  Sys_Tick++;
}

void systick_start(unsigned int us)
{
	systick_set_reload(us * SYSTICK_MULTIPLIER);

	systick_interrupt_enable();
	/* start counting */
	systick_counter_enable();
}

void systick_stop(void)
{
	/* stop counting */
	systick_counter_disable();
	systick_interrupt_disable();
}

void systick_wait(void)
{
  Sys_Tick = 0;
  while (!Sys_Tick)
#ifndef DEBUG
    __wfi();
#else
	  ;
#endif
}

void delay(unsigned int us)
{
  systick_start(us);
  systick_wait();
  systick_stop();
}

void delayms(unsigned int ms)
{
  while (ms--)
    delay(1000);
}

void ETH_IRQHandler(void)
{
  ETH_InterruptHandler();
}
