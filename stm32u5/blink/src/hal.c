#include "board.h"
#include <rcc.h>
#include <gpio.h>

const RCCConfig rcc_config =
{
  .hse_frequency = 8000000,
  .hsebypass = 1,
  .pll = {
    {.m = 1, .p_frequency = 0, .q_frequency = 0, .r_frequency = 48000000},
    {.m = 1, .p_frequency = 0, .q_frequency = 0, .r_frequency = 0},
    {.m = 1, .p_frequency = 0, .q_frequency = 0, .r_frequency = 0},
  },
  .hpre = 1,
  .ppre1 = 1,
  .ppre2 = 1,
  .ppre3 = 1,
  .dpre = 1,
  .main_clock_source = RCC_CFGR1_SW_0 | RCC_CFGR1_SW_1, // PLL1P
  .ahb1dis = false,
  .ahb2dis1 = false,
  .ahb2dis2 = false,
  .ahb3dis = false,
  .apb1dis = false,
  .apb2dis = false,
  .apb3dis = false,
  .boosten = false,
  .flash_pdreq1 = false,
  .flash_pdreq2 = false,
  .flash_sleep_pd = false,
  .usbhsboosten = false,
  .usbhspwren = false,
  .vdd11usbdis = false
};

static void GPIOInit(void)
{
  GPIO_InitTypeDef init;

  RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOCEN | RCC_AHB2ENR1_GPIOBEN | RCC_AHB2ENR1_GPIOGEN;

  PWR->SVMCR |= PWR_SVMCR_IO2SV; // enable portg[15:2]

  LED_GREEN_OFF;
  init.Pin = GPIO_Pin_7;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(GPIOC, &init);
  LED_BLUE_OFF;
  GPIO_Init(GPIOB, &init);
  LED_RED_OFF;
  init.Pin = GPIO_Pin_2;
  GPIO_Init(GPIOG, &init);
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
