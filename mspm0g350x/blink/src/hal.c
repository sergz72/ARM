#include "board.h"
#include <dl_gpio.h>
#include <m0p/dl_sysctl.h>
#include <m0p/dl_systick.h>

static volatile int Sys_Tick;

void SysTick_Handler(void)
{
  Sys_Tick++;
}

static void systick_start(unsigned int us)
{
  DL_SYSTICK_setPeriod(us * SYSTICK_MULTIPLIER);

  DL_SYSTICK_enableInterrupt();
  /* start counting */
  DL_SYSTICK_enable();
}

static void systick_stop(void)
{
  /* stop counting */
  DL_SYSTICK_disable();
  DL_SYSTICK_disableInterrupt();
}

static void systick_wait(void)
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

static void ClocksInit(void)
{
  // SYSOSC = 32 MHz = 1 wait state
  DL_SYSCTL_setFlashWaitState(DL_SYSCTL_FLASH_WAIT_STATE_1);
}

static void GPIOInit(void)
{
  DL_GPIO_enablePower(GPIOA);
  DL_GPIO_initDigitalOutput(LED_IOMUX);
  DL_GPIO_clearPins(LED_GPIO, LED_PIN);
  DL_GPIO_enableOutput(LED_GPIO, LED_PIN);
}

void SystemInit(void)
{
  ClocksInit();
  GPIOInit();
}
