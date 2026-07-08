#include "board.h"
#include <systick.h>
#include <m0p/dl_systick.h>

static volatile int Sys_Tick;

void SysTick_Handler(void)
{
  Sys_Tick++;
}

void systick_start(unsigned int us)
{
  DL_SYSTICK_setPeriod(us * SYSTICK_MULTIPLIER);

  DL_SYSTICK_enableInterrupt();
  /* start counting */
  DL_SYSTICK_enable();
}

void systick_stop(void)
{
  /* stop counting */
  DL_SYSTICK_disable();
  DL_SYSTICK_disableInterrupt();
}

void systick_wait(void)
{
  Sys_Tick = 0;
  while (!Sys_Tick)
    __WFI();
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
