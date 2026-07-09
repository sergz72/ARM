#include "board.h"
#include <rcc.h>
#include <gpio.h>
#include <usb.h>
#include <nvic.h>

const RCCConfig rcc_config =
{
  .hse_frequency = 25000000,
  .hsebypass = 0,
  .pll = {
    {.m = 5, .p_frequency = 0, .q_frequency = 48000000, .r_frequency = 48000000},
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

  RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOCEN;

  PWR->SVMCR |= PWR_SVMCR_IO2SV; // enable portg[15:2]

  LED_OFF;
  init.Pin = GPIO_Pin_13;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(GPIOC, &init);
}

static void USB_Init(void)
{
  GPIO_InitTypeDef init;

  RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOAEN;

  // Configure DM DP Pins
  init.Pin = GPIO_Pin_11 | GPIO_Pin_12;
  init.Mode = GPIO_MODE_AF_PP;
  init.Pull = GPIO_NOPULL;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Alternate = GPIO_AF10_OTG_FS;
  GPIO_Init(GPIOA, &init);

  PWR->SVMCR |= PWR_SVMCR_USV;

  RCC->CCIPR1 |= RCC_CCIPR1_ICLKSEL_1; // pll1_q as usb clock
  // Enable USB FS Clock
  RCC->AHB2ENR1 |= RCC_AHB2ENR1_OTGEN;

  NVIC_Init(OTG_FS_IRQn, 7, 0, ENABLE);
}

void SystemInit(void)
{
  if (InitRCC(&rcc_config))
  {
    while (1)
      ;
  }

  GPIOInit();
  USB_Init();
}

void _init(void)
{

}

void OTG_FS_IRQHandler(void)
{
  USBInterruptHandler();
}
