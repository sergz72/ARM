#include "board.h"
#include <pll.h>
#include <systick.h>
#include <gpio.h>

const RCCConfig rcc_config =
{
  .hse_frequency = 8000000,
  .hsebypass = 0,
  .pllm = 4,
  .pllp_frequency = 48000000,
  .pllq_frequency = 48000000,
  .hpre = 1,
  .ppre1 = 1,
  .ppre2 = 1,
  .ppre3 = 1
};

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

static void GPIOInit(void)
{
  GPIO_InitTypeDef init;

  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

  LED_OFF;
  init.Pin = GPIO_Pin_13;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  // LED
  GPIO_Init(GPIOC, &init);
}

void SystemInit(void)
{
  if (InitRCC(&rcc_config))
  {
    while (1)
      ;
  }

  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);

  GPIOInit();
}

void _init(void)
{

}
