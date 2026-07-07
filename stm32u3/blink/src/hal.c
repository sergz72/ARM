#include "board.h"
#include <rcc.h>
#include <gpio.h>

const RCCConfig rcc_config =
{
  .hse_frequency = 8000000,
  .hsebypass = 1,
  .hpre = 1,
  .ppre1 = 1,
  .ppre2 = 1,
  .ppre3 = 1,
  .main_clock_source = 0, // MSIS
  .boosten = false,
  .flash_pdreq1 = false,
  .flash_pdreq2 = false,
  .flash_sleep_pd = false
};

static void GPIOInit(void)
{
  GPIO_InitTypeDef init;

  RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOAEN;

  LED_OFF;
  init.Pin = GPIO_Pin_5;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(GPIOA, &init);
}

void SystemInit(void)
{
  if (InitRCC(&rcc_config))
  {
    while (1)
      ;
  }

  GPIOInit();
}

void _init(void)
{

}
