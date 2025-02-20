#include "hal.h"
#include <gpio.h>
#include <exti.h>
#include <nvic.h>
#include <spi.h>
#include <timer.h>

#define ADC_VREF 33000 //x0.1mv

const SPI_InitTypeDef spi_slave_typedef = {
  .Direction = SPI_DIRECTION_2LINES,
  .Mode = SPI_MODE_SLAVE,
  .DataSize = 8,
  .CLKPolarity = SPI_POLARITY_LOW,
  .CLKPhase = SPI_PHASE_1EDGE,
  .NSS = SPI_NSS_HARD_INPUT,
  .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32,
  .FirstBit = SPI_FIRSTBIT_MSB,
  .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
  .CRCPolynomial = 7,
  .TIMode = SPI_TIMODE_DISABLE,
  .InterruptsToEnable = SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE
};

static unsigned char *rxbuf_p, *txbuf_p;
static unsigned int spi1_cr1, spi1_cr2;

static inline void pointers_reset(void *rxaddress, const void *txaddress)
{
  rxbuf_p = (unsigned char*)rxaddress;
  txbuf_p = (unsigned char*)txaddress;
}

static void SPISlaveReInit(void)
{
  SPI1_Reset();

  SPI1->CR1 = spi1_cr1;
  SPI1->CR2 = spi1_cr2;

  SPI_Enable(SPI1);
}

void __attribute__((section(".RamFunc"))) EXTI4_15_IRQHandler(void)
{
  //while ((SPI1->SR & SPI_SR_FRLVL) != 0)
  //  *rxbuf_p++ = (unsigned char)SPI1->DR;
  pointers_reset(rxbuf, txbufs[rxbuf[0] & 7]);
  SPISlaveReInit();
  command_ready = 1;
  EXTI->RPR1 = EXTI_Line4;
}

void __attribute__((section(".RamFunc"))) TIM14_IRQHandler(void)
{
  if (TIM14->SR & TIM_IT_Update)
  {
    timer_interrupt = 1;
    TIM14->SR = ~TIM_IT_Update;
  }
}

void __attribute__((section(".RamFunc"))) SPI1_IRQHandler(void)
{
  while ((SPI1->SR & SPI_SR_FTLVL) != SPI_SR_FTLVL)
    *(unsigned char*)&SPI1->DR = *txbuf_p++;
  while ((SPI1->SR & SPI_SR_FRLVL) != 0)
    *rxbuf_p++ = (unsigned char)SPI1->DR;
}

/*
 * LED_TIMER   = PA1
 * LED_COMMAND = PA2
 */

static void GPIOInit(void)
{
  //LEDS
  GPIO_Init(GPIOA,
            GPIO_Pin_1 | GPIO_Pin_2,
            GPIO_Mode_OUT,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

/*
 *PA4 = SPI1_NSS
 *PA5 = SPI1_SCK
 *PA6 = SPI1_MISO
 *PA7 = SPI1_MOSI
 */

static void SPISlaveInit(void)
{
  RCC->APBENR2 |= RCC_APBENR2_SPI1EN;

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);
  //SCK,MISO,MOSI -> pulldown
  GPIO_Init(GPIOA,
            GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7,
            GPIO_Mode_AF,
            GPIO_Speed_VeryHigh,
            GPIO_OType_PP,
            GPIO_PuPd_DOWN
  );
  //NSS -> pullup
  GPIO_Init(GPIOA,
            GPIO_Pin_4,
            GPIO_Mode_AF,
            GPIO_Speed_VeryHigh,
            GPIO_OType_PP,
            GPIO_PuPd_UP
  );

  SPI_Init(SPI1, &spi_slave_typedef);
  spi1_cr1 = SPI1->CR1;
  spi1_cr2 = SPI1->CR2;

  SPI_Enable(SPI1);

  NVIC_Init(SPI1_IRQn, 0, 0, ENABLE);
}

/*
 *PA8 = SPI2_NSS
 *PA0 = SPI2_SCK
 *PB7 = SPI2_MOSI
 */

static void SPIMasterInit(void)
{
  SPI_InitTypeDef SPI_InitStructure;

  RCC->APBENR1 |= RCC_APBENR1_SPI2EN;

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_0);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_1);
  //SCK, MOSI -> pulldown
  GPIO_Init(GPIOA,
            GPIO_Pin_0,
            GPIO_Mode_AF,
            GPIO_Speed_VeryHigh,
            GPIO_OType_PP,
            GPIO_PuPd_DOWN
  );
  GPIO_Init(GPIOB,
            GPIO_Pin_7,
            GPIO_Mode_AF,
            GPIO_Speed_VeryHigh,
            GPIO_OType_PP,
            GPIO_PuPd_DOWN
  );
  //NSS -> pullup
  GPIO_Init(GPIOA,
            GPIO_Pin_8,
            GPIO_Mode_AF,
            GPIO_Speed_VeryHigh,
            GPIO_OType_PP,
            GPIO_PuPd_UP
  );

  SPI_InitStructure.Direction = SPI_DIRECTION_2LINES;
  SPI_InitStructure.Mode = SPI_MODE_MASTER;
  SPI_InitStructure.DataSize = 16;
  SPI_InitStructure.CLKPolarity = SPI_POLARITY_LOW;
  SPI_InitStructure.CLKPhase = SPI_PHASE_1EDGE;
  SPI_InitStructure.NSS = SPI_NSS_HARD_OUTPUT;
  SPI_InitStructure.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  SPI_InitStructure.FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  SPI_InitStructure.CRCPolynomial = 7;
  SPI_InitStructure.TIMode = SPI_TIMODE_DISABLE;

  SPI_Init(SPI2, &SPI_InitStructure);
  SPI_Enable(SPI2);
}

/*
 *PA4 = SPI1_NSS
 */

static void EXTIInit(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  EXTI_LineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);

  EXTI_ClearITPendingBit(EXTI_Line4);
  EXTI_InitStructure.EXTI_Line = EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_Init(EXTI4_15_IRQn, 1, 0, ENABLE);
}

/*
 *PA3 = ADC1_IN3
 */

static void ADCInit(void)
{
  GPIO_Init(GPIOA,
            GPIO_Pin_3,
            GPIO_Mode_AN,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

static void TimerInit(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  //enable clock for TIM14
  RCC->APBENR2 |= RCC_APBENR2_TIM14EN;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 5000; // 5000 us
  TIM_TimeBaseStructure.TIM_Prescaler = (unsigned short) (BOARD_PCLK2 / 1000000) - 1; // tick every 1 us
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

  TIM_ARRPreloadConfig(TIM14, ENABLE);

  TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);

  NVIC_Init(TIM14_IRQn, 2, 0, ENABLE);
}

// Init system clock (48 MHz)
static void ClockInit(void)
{
  FLASH->ACR |= FLASH_ACR_LATENCY_1; // 1 wait state up to 48 MHz
  //PLLM = 2, PLLN = 12, PLLR = 2
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSI | RCC_PLLCFGR_PLLM_0 * (PLLM - 1) | RCC_PLLCFGR_PLLN_0 * PLLN |
                  RCC_PLLCFGR_PLLR_0 * (PLLR - 1) | RCC_PLLCFGR_PLLREN;
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;
  RCC->CFGR = RCC_CFGR_SW_1;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLLRCLK)
    ;
}

void SystemInit(void)
{
//#ifdef DEBUG
//  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE);
//#endif

  ClockInit();

  RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;
  RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;

  GPIOInit();
  //SPIMasterInit();
  //ADCInit();
  TimerInit();
}

void SysInit(void *rxaddress, const void *txaddress)
{
  pointers_reset(rxaddress, txaddress);
  SPISlaveInit();
  EXTIInit();
}

unsigned short adc_get(void)
{
  //todo
  return 0;
}

void timer_enable(void)
{
  TIM_Cmd(TIM14, ENABLE);
}

void status_updated(void)
{
}

void ad9833_write(int channel, unsigned short data)
{
  //todo
}

void _init(void)
{

}