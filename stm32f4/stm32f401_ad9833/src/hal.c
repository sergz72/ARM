#include "hal.h"
#include <dbgmcu.h>
#include <pll.h>
#include <gpio.h>
#include <exti.h>
#include <nvic.h>
#include <spi.h>
#include <timer.h>
#include <dma.h>

#define ADC_VREF 33000 //x0.1mv

#ifdef SPI_SLAVE_DMA
static inline void pointers_reset(void *rxaddress, const void *txaddress)
{
  DMA1_Stream3->CR &= ~DMA_SxCR_EN;
  DMA1_Stream4->CR &= ~DMA_SxCR_EN;

  DMA1_Stream3->NDTR = MAX_TRANSFER_SIZE;
  DMA1_Stream3->M0AR = (unsigned int)rxaddress;
  DMA1_Stream4->NDTR = MAX_TRANSFER_SIZE;
  DMA1_Stream4->M0AR = (unsigned int)txaddress;

  DMA1_Stream3->CR |= DMA_SxCR_EN;
  DMA1_Stream4->CR |= DMA_SxCR_EN;
}

#else
static unsigned char *rxbuf_p, *txbuf_p;

static inline void pointers_reset(void *rxaddress, const void *txaddress)
{
  rxbuf_p = (unsigned char*)rxaddress;
  txbuf_p = (unsigned char*)txaddress;
}
#endif

void __attribute__((section(".RamFunc"))) EXTI15_10_IRQHandler(void)
{
  pointers_reset(rxbuf, txbufs[rxbuf[0] & 7]);
#ifndef SPI_SLAVE_DMA
  SPI2->DR = *txbuf_p++;
#endif
  command_ready = 1;
  EXTI->PR = EXTI_Line12;
}

void __attribute__((section(".RamFunc"))) TIM2_IRQHandler(void)
{
  if (TIM2->SR & TIM_IT_Update)
  {
    timer_interrupt = 1;
    TIM2->SR = ~TIM_IT_Update;
  }
}

#ifndef SPI_SLAVE_DMA
void __attribute__((section(".RamFunc"))) SPI2_IRQHandler(void)
{
  unsigned int status = SPI2->SR;
  if (status & SPI_SR_TXE)
    SPI2->DR = *txbuf_p++;
  if (status & SPI_SR_RXNE)
    *rxbuf_p++ = SPI2->DR;
}
#endif

/*
 * LED_TIMER   = PC13
 * LED_COMMAND = PB10
 */

static void GPIOInit(void)
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
  //LEDS
  GPIO_Init(GPIOC,
            GPIO_Pin_13,
            GPIO_Mode_OUT,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_Init(GPIOB,
            GPIO_Pin_10,
            GPIO_Mode_OUT,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

/*
 *PB12 = SPI2_NSS
 *PB13 = SPI2_SCK
 *PB14 = SPI2_MISO
 *PB15 = SPI2_MOSI
 */

#ifdef SPI_SLAVE_DMA
static void DMAInit(void *rxaddress, const void *txaddress)
{
  DMA_InitTypeDef init;

  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

  init.DMA_Mode = DMA_Mode_Normal;
  init.DMA_Channel = DMA_Channel_0;
  init.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  init.DMA_FIFOMode = DMA_FIFOMode_Disable;
  init.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  init.DMA_Memory0BaseAddr = (uint32_t)txaddress;
  init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  init.DMA_MemoryInc = DMA_MemoryInc_Enable;
  init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  init.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;
  init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  init.DMA_Priority = DMA_Priority_VeryHigh;
  init.DMA_BufferSize = MAX_TRANSFER_SIZE;
  DMA_Init(DMA1_Stream4, &init);
  init.DMA_DIR = DMA_DIR_PeripheralToMemory;
  init.DMA_Memory0BaseAddr = (uint32_t)rxaddress;
  DMA_Init(DMA1_Stream3, &init);
  DMA_FlowControllerConfig(DMA1_Stream4, DMA_FlowCtrl_Peripheral);
  DMA_FlowControllerConfig(DMA1_Stream3, DMA_FlowCtrl_Peripheral);

  DMA_Cmd(DMA1_Stream4, ENABLE);
  DMA_Cmd(DMA1_Stream3, ENABLE);
}
#endif

static void SPISlaveInit(void)
{
  SPI_InitTypeDef SPI_InitStructure;

  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
  //SCK,MISO,MOSI -> pulldown
  GPIO_Init(GPIOB,
            GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_DOWN
  );
  //NSS -> pullup
  GPIO_Init(GPIOB,
            GPIO_Pin_12,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_UP
  );

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI2, &SPI_InitStructure);

#ifdef SPI_SLAVE_DMA
  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
#else
  SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_RXNE);
  SPI_I2S_ClearITPendingBit(SPI2, SPI_I2S_IT_TXE);
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);
#endif

  SPI_Cmd(SPI2, ENABLE);

#ifndef SPI_SLAVE_DMA
  NVIC_Init(SPI2_IRQn, 0, 0, ENABLE);
#endif
}

/*
 *PA4 = SPI1_NSS
 *PA5 = SPI1_SCK
 *PA7 = SPI1_MOSI
 */

static void SPIMasterInit(void)
{
  SPI_InitTypeDef SPI_InitStructure;

  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
  //SCK, MOSI -> pulldown
  GPIO_Init(GPIOA,
            GPIO_Pin_5 | GPIO_Pin_7,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_DOWN
  );
  //NSS -> pullup
  GPIO_Init(GPIOA,
            GPIO_Pin_4,
            GPIO_Mode_AF,
            GPIO_Speed_100MHz,
            GPIO_OType_PP,
            GPIO_PuPd_UP
  );

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;

  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_Cmd(SPI1, ENABLE);
}

/*
 *PB12 = SPI2_NSS
 */

static void EXTIInit(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;

  EXTI_LineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);

  EXTI_ClearITPendingBit(EXTI_Line12);
  EXTI_InitStructure.EXTI_Line = EXTI_Line12;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  NVIC_Init(EXTI15_10_IRQn, 1, 0, ENABLE);
}

/*
 *PA3 = ADC1_IN3
 */

static void ADCInit(void)
{
  GPIO_Init(GPIOA,
            GPIO_Pin_3,
            GPIO_Mode_AN,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

static void TimerInit(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  //enable clock for TIM2
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 5000; // 5000 us
  TIM_TimeBaseStructure.TIM_Prescaler = (unsigned short) (BOARD_PCLK1 / 500000) - 1; // tick every 1 us
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  TIM_ARRPreloadConfig(TIM2, ENABLE);

  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  NVIC_Init(TIM2_IRQn, 2, 0, ENABLE);
}

void SystemInit(void)
{
#ifdef DEBUG
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE);
#endif

  // Init system clock (48 MHz)
  InitRCC(25, 48, FLASH_ACR_LATENCY_1WS, RCC_CFGR_HPRE_DIV1, RCC_CFGR_PPRE2_DIV2, RCC_CFGR_PPRE1_DIV2);

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  GPIOInit();
  SPIMasterInit();
  EXTIInit();
  ADCInit();
  TimerInit();
}

void SysInit(void *rxaddress, const void *txaddress)
{
#ifdef SPI_SLAVE_DMA
  DMAInit(rxaddress, txaddress);
#else
  pointers_reset(rxaddress, txaddress);
#endif
  SPISlaveInit();
}

unsigned short adc_get(void)
{
  //todo
  return 0;
}

void timer_enable(void)
{
  TIM_Cmd(TIM2, ENABLE);
}

void status_updated(void)
{
}

void ad9833_write(int channel, unsigned short data)
{
  //todo
}
