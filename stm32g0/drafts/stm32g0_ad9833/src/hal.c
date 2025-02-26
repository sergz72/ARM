#include "hal.h"
#include <gpio.h>
#include <exti.h>
#include <nvic.h>
#include <spi.h>
#include <timer.h>
#include <systick.h>
#include <adc.h>
#include <string.h>

#include "generic_dds.h"

#define ADC_VREF 33000 //x0.1mv

const SPI_InitTypeDef spi_slave_init = {
  .Direction = SPI_DIRECTION_2LINES,
  .Mode = SPI_MODE_SLAVE,
  .DataSize = 8,
  .CLKPolarity = SPI_POLARITY_LOW,
  .CLKPhase = SPI_PHASE_1EDGE,
  .NSS = SPI_NSS_HARD_INPUT,
  .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
  .FirstBit = SPI_FIRSTBIT_MSB,
  .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
  .CRCPolynomial = 7,
  .TIMode = SPI_TIMODE_DISABLE,
#ifdef SPI_SLAVE_DMA
  .InterruptsToEnable = 0,
  .DMAToEnable = SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN | SPI_CR2_FRXTH
#else
  .InterruptsToEnable = SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_ERRIE,
  .DMAToEnable = 0
#endif
};

const SPI_InitTypeDef spi_master_init = {
  .Direction = SPI_DIRECTION_2LINES,
  .Mode = SPI_MODE_MASTER,
#ifdef DDS_TYPE_AD9833
  .DataSize = 16,
  .CLKPolarity = SPI_POLARITY_HIGH,
#elifdef DDS_TYPE_ADF4351
  .DataSize = 16,
  .CLKPolarity = SPI_POLARITY_LOW,
#else
  .DataSize = 8,
  .CLKPolarity = SPI_POLARITY_LOW,
#endif
  .CLKPhase = SPI_PHASE_1EDGE,
  .NSS = SPI_NSS_SOFT,
  .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
#if defined(DDS_TYPE_AD9833) || defined(DDS_TYPE_ADF4351)
  .FirstBit = SPI_FIRSTBIT_MSB,
#else
  .FirstBit = SPI_FIRSTBIT_LSB,
#endif
  .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
  .CRCPolynomial = 7,
  .TIMode = SPI_TIMODE_DISABLE,
  .InterruptsToEnable = 0,
  .DMAToEnable = 0
};

volatile int command_to_process;

static unsigned int spi1_cr1, spi1_cr2;

volatile int Sys_Tick;

static unsigned char rxbuf[MAX_TRANSFER_SIZE];

void SysTick_Handler(void)
{
  Sys_Tick++;
}

static void systick_start(unsigned int us)
{
  systick_set_reload(us * SYSTICK_MULTIPLIER);

  systick_interrupt_enable();
  /* start counting */
  systick_counter_enable();
}

static void systick_stop(void)
{
  /* stop counting */
  systick_counter_disable();
  systick_interrupt_disable();
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

#ifdef SPI_SLAVE_DMA
static void inline DMADisable(void)
{
  DMA1_Channel1->CCR &= ~DMA_CCR_EN;
  DMA1_Channel2->CCR &= ~DMA_CCR_EN;
}

static void inline DMAReinit(void *rxaddress, const void *txaddress)
{
  DMA1_Channel1->CMAR = (unsigned int)rxaddress;
  DMA1_Channel1->CNDTR = MAX_TRANSFER_SIZE;

  DMA1_Channel2->CMAR = (unsigned int)txaddress;
  DMA1_Channel2->CNDTR = MAX_TRANSFER_SIZE;

  DMA1_Channel1->CCR |= DMA_CCR_EN;
  DMA1_Channel2->CCR |= DMA_CCR_EN;
}
#else
static unsigned char *rxbuf_p, *txbuf_p;

static inline void pointers_reset(void *rxaddress, const void *txaddress)
{
  rxbuf_p = (unsigned char*)rxaddress;
  txbuf_p = (unsigned char*)txaddress;
}
#endif

static void inline SPISlaveReInit(void)
{
  SPI1_Reset();

  SPI1->CR1 = spi1_cr1;
  SPI1->CR2 = spi1_cr2;
}

void __attribute__((section(".RamFunc"))) EXTI4_15_IRQHandler(void)
{
#ifdef SPI_SLAVE_DMA
  DMADisable();
#else
  pointers_reset(rxbuf, txbufs[rxbuf[0] & 7]);
#endif
  SPISlaveReInit();
#ifdef SPI_SLAVE_DMA
  DMAReinit(rxbuf, txbufs[rxbuf[0] & 7]);
#endif
  SPI_Enable(SPI1);
  memcpy(&commands[command_to_process], rxbuf, sizeof(dds_i2c_command));
  if (command_to_process == DDS_COMMAND_QUEUE_LENGTH - 1)
    command_to_process = 0;
  else
    command_to_process++;
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

#ifndef SPI_SLAVE_DMA
void __attribute__((section(".RamFunc"))) SPI1_IRQHandler(void)
{
  while ((SPI1->SR & SPI_SR_FTLVL) != SPI_SR_FTLVL)
    *(unsigned char*)&SPI1->DR = *txbuf_p++;
  while ((SPI1->SR & SPI_SR_FRLVL) != 0)
    *rxbuf_p++ = (unsigned char)SPI1->DR;
}
#endif

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
  //INTERRUPT FLAG
  INTERRUPT_FLAG_CLR;
  GPIO_Init(GPIOA,
            GPIO_Pin_11,
            GPIO_Mode_OUT,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
#ifdef DDS_TYPE_AD9850
  AD9850_RESET_SET(0);
  GPIO_Init(GPIOA,
            GPIO_Pin_12,
            GPIO_Mode_OUT,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
#endif
#ifdef DDS_TYPE_ADF4351
  ADF4351_CE_SET(0);
  GPIO_Init(GPIOA,
            GPIO_Pin_12,
            GPIO_Mode_OUT,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
#endif
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

  SPI_Init(SPI1, &spi_slave_init);
  spi1_cr1 = SPI1->CR1;
  spi1_cr2 = SPI1->CR2;

  SPI_Enable(SPI1);

#ifndef SPI_SLAVE_DMA
  NVIC_Init(SPI1_IRQn, 0, 0, ENABLE);
#endif
}

#ifdef SPI_SLAVE_DMA
static void DMAInit(void *rxaddress, const void *txaddress)
{
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  // DMA channel 1 = from SPI to mem
  DMA1_Channel1->CCR = DMA_CCR_MINC | // memory increment mode
                       DMA_CCR_PL; // very high priority level
  DMA1_Channel1->CPAR = (unsigned int)&SPI1->DR;

  // DMA channel 2 = from mem to SPI
  DMA1_Channel2->CCR = DMA_CCR_MINC | // memory increment mode
                       DMA_CCR_PL | // very high priority level
                       DMA_CCR_DIR; // read from memory
  DMA1_Channel2->CPAR = (unsigned int)&SPI1->DR;

  // 16 = SPI1_RX
  DMAMUX1_Channel0->CCR = 16;
  // 17 = SPI1_TX
  DMAMUX1_Channel1->CCR = 17;

  DMAReinit(rxaddress, txaddress);
}
#endif

/*
 *PA8 = SPI2_NSS
 *PA0 = SPI2_SCK
 *PB7 = SPI2_MOSI
 */

static void SPIMasterInit(void)
{
  RCC->APBENR1 |= RCC_APBENR1_SPI2EN;

  //GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_1);
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
#if defined(DDS_TYPE_AD9833) || defined(DDS_TYPE_ADF4351)
  SPI2_NSS_SET;
#else
  SPI2_NSS_CLR;
#endif
  GPIO_Init(GPIOA,
            GPIO_Pin_8,
            GPIO_Mode_OUT,
            GPIO_Speed_VeryHigh,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  SPI_Init(SPI2, &spi_master_init);
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

  ADC_Init(0, 4);
  ADC_Enable();
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

  /* 48MHz / 8 => 6M counts per second */
  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB);

  RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;
  RCC->APBENR2 |= RCC_APBENR2_SYSCFGEN;

  GPIOInit();
  SPIMasterInit();
  ADCInit();
  TimerInit();
}

void SysInit(const void *txaddress)
{
  command_to_process = 0;
#ifdef SPI_SLAVE_DMA
  DMAInit(rxbuf, txaddress);
#else
  pointers_reset(rxaddress, txaddress);
#endif
  SPISlaveInit();
  EXTIInit();
}

unsigned short adc_get(void)
{
  return (unsigned short)ADC_GetValue(3, ADC_SampleTime_160_5Cycles);
}

void timer_enable(void)
{
  TIM_Cmd(TIM14, ENABLE);
}

void status_updated(void)
{
  if (status)
    INTERRUPT_FLAG_SET;
  else
    INTERRUPT_FLAG_CLR;
}

#ifdef DDS_TYPE_AD9833
void ad9833_write(int channel, unsigned short data)
{
  SPI2_NSS_CLR;
  SPI2->DR = data;
  while (SPI2->SR & SPI_SR_BSY)
    ;
  volatile unsigned short dummy = SPI2->DR;
  (void)dummy;
  SPI2_NSS_SET;
}
#endif

#ifdef DDS_TYPE_AD9850
void ad9850_write(int channel, const unsigned char *data, unsigned int length)
{
  while (length--)
  {
    while ((SPI2->SR & SPI_SR_FRLVL) != 0)
    {
      volatile unsigned char dummy = SPI2->DR;
      (void)dummy;
    }
    while ((SPI2->SR & SPI_SR_FTLVL) == SPI_SR_FTLVL)
      ;
    *(unsigned char*)&SPI2->DR = *data++;
  }
  while (SPI2->SR & SPI_SR_BSY)
  {
    while ((SPI2->SR & SPI_SR_FRLVL) != 0)
    {
      volatile unsigned char dummy = SPI2->DR;
      (void)dummy;
    }
  }
  SPI2_NSS_SET;
  AD9850_DELAY;
  SPI2_NSS_CLR;
}
#endif

#ifdef DDS_TYPE_ADF4351
void adf4351_write(int channel, unsigned int data)
{
  unsigned short *sdata = (unsigned short *)&data;

  SPI2_NSS_CLR;
  SPI2->DR = sdata[1];
  SPI2->DR = sdata[0];
  while (SPI2->SR & SPI_SR_BSY)
  {
    while ((SPI2->SR & SPI_SR_FRLVL) != 0)
    {
      volatile unsigned char dummy = SPI2->DR;
      (void)dummy;
    }
  }
  SPI2_NSS_SET;
}
#endif

void dummy_call(void)
{
  asm("nop");
}

void _init(void)
{

}
