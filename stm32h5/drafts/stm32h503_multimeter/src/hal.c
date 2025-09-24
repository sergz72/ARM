#include "board.h"
#include <pll.h>
#include <nvic.h>

const RCCConfig rcc_config =
{
  .hse_frequency = 8000000,
  .hsebypass = 0,
  .pllm = 1,
  .pllp_frequency = 240000000,
  .pllq_frequency = 48000000,
  .hpre = 1,
  .ppre1 = 1,
  .ppre2 = 1,
  .ppre3 = 1
};

volatile int capacity_measurement_done;

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

  init.Pin = GPIO_Pin_1 | GPIO_Pin_2;
  init.Mode = GPIO_MODE_INPUT;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_PULLDOWN;
  // BUTTONs
  GPIO_Init(GPIOB, &init);

  //clr discharge pin
  R_PORT->BSRR = R_DISCHARGE_PIN<<16;
  CHARGE_OFF;

  init.Pin = R_CHARGE_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(R_PORT, &init);
  discharge_on();
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

static void TIM2Init(void)
{
  GPIO_InitTypeDef init;

  RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;

  // input capture pins: PA3=CH4, PA5=CH1
  init.Pin = CAPTURE_PIN_CHARGE | CAPTURE_PIN_CX;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  init.Alternate = GPIO_AF1_TIM2;
  GPIO_Init(CAPTURE_PORT, &init);

  TIM2->PSC = 0;
  TIM2->ARR = 0xFFFFFFFF;
  //no filter, no prescaler, CC1 channel is configured as input, tim_ic1 is mapped on tim_ti1
  TIM2->CCMR1 = TIM_CCMR1_CC1S_0;
  //no filter, no prescaler, CC4 channel is configured as input, tim_ic4 is mapped on tim_ti4
  TIM2->CCMR2 = TIM_CCMR2_CC4S_0;
  //rising-edge capture mode for all channels
  TIM2->CCER = TIM_CCER_CC1E | TIM_CCER_CC4E;

  NVIC_Init(TIM2_IRQn, TIMER_INTERRUPT_PRIORITY, 0, ENABLE);
}

void SystemInit(void)
{
  capacity_measurement_done = 0;

  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOAEN;

  GPIOInit();

  if (InitRCC(&rcc_config))
  {
    LED_ON;
    while (1)
      ;
  }

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  USB_Init();
  TIM2Init();
}

void _init(void)
{
}

void discharge_off(void)
{
  GPIO_InitTypeDef init;

  init.Pin = R_DISCHARGE_PIN;
  init.Mode = GPIO_MODE_ANALOG;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(R_PORT, &init);
}

void discharge_on(void)
{
  GPIO_InitTypeDef init;

  init.Pin = R_DISCHARGE_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(R_PORT, &init);
}

unsigned int get_capacity_measurement_start_time(void)
{
  return TIM2->CCR4;
}

unsigned int get_capacity_measurement_end_time(void)
{
  return TIM2->CCR1;
}

void capacity_measurement_start(void)
{
  discharge_off();
  TIM2->CNT = 0;
  TIM2->DIER = TIM_DIER_CC1IE;
  TIM2->SR = 0;
  //start TIM2
  TIM2->CR1 = TIM_CR1_CEN;
  CHARGE_ON;
}

void TIM2_IRQHandler(void)
{
  capacity_measurement_done = 1;
  //stop timer
  TIM2->CR1 = 0;
  //disable TIM2 interrupts
  TIM2->DIER = 0;
  //clear interrupt flags
  TIM2->SR = 0;
}
