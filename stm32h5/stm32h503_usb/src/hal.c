#include "board.h"
#include <pll.h>
#include <gpio.h>
#include <nvic.h>

const RCCConfig rcc_config =
{
  .hse_frequency = 8000000,
  .hsebypass = 0,
  .pllm = 1,
  .pllp_frequency = 48000000,
  .pllq_frequency = 48000000,
  .hpre = 1,
  .ppre1 = 1,
  .ppre2 = 1,
  .ppre3 = 1
};

static void GPIOInit(void)
{
  GPIO_InitTypeDef init;

  LED_OFF;
  init.Pin = GPIO_Pin_13;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  // LED
  GPIO_Init(GPIOC, &init);
}

static void USB_Init(void)
{
  GPIO_InitTypeDef init;
  //PA11=USBDM,PA12=USBDP
  init.Pin = GPIO_Pin_11 | GPIO_Pin_12;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  init.Alternate = GPIO_AF10_USB;
  GPIO_Init(GPIOA, &init);

  RCC->CCIPR4 |= RCC_CCIPR4_USBSEL_0; // pll1_q_ck selected as usb kernel clock
  RCC->APB2ENR |= RCC_APB2ENR_USBEN;

  NVIC_Init(USB_DRD_FS_IRQn, 1, 3, ENABLE);
}

void SystemInit(void)
{
  if (InitRCC(&rcc_config))
  {
    while (1)
      ;
  }

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOAEN;// | RCC_AHB2ENR_GPIOBEN;

  GPIOInit();
  USB_Init();
}

void _init(void)
{

}
