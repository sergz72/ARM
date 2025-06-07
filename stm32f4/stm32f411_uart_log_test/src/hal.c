#include "board.h"
#include <dbgmcu.h>
#include <pll.h>
#include <gpio.h>
#include <uart_log.h>
#include <dma.h>

static void GPIOInit(void)
{
  //LED
  GPIO_Init(GPIOC,
            GPIO_Pin_13,
            GPIO_Mode_OUT,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

/*
 * PA2=USART2_TX
 * PA3=USART2_RX
 */

static void USART2Init(void)
{
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
  GPIO_Init(GPIOA,
            GPIO_Pin_2 | GPIO_Pin_3,
            GPIO_Mode_AF,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  USART2->BRR = BOARD_PCLK1 / USART_BAUD_RATE;
  //USART2->CR2 = 0;
  USART2->CR3 = USART_CR3_DMAT;
  USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

static void DMAInitForUSART2(void)
{
  DMA_InitTypeDef init;

  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

  init.DMA_Mode = DMA_Mode_Normal;
  init.DMA_Channel = DMA_Channel_4;
  init.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  init.DMA_FIFOMode = DMA_FIFOMode_Disable;
  init.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  //init.DMA_Memory0BaseAddr = (uint32_t)txaddress;
  init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  init.DMA_MemoryInc = DMA_MemoryInc_Enable;
  init.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  init.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
  init.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  init.DMA_Priority = DMA_Priority_Low;
  //init.DMA_BufferSize = MAX_TRANSFER_SIZE;
  DMA_Init(DMA1_Stream6, &init);
  DMA_FlowControllerConfig(DMA1_Stream6, DMA_FlowCtrl_Memory);
}

void SystemInit(void)
{
#ifdef DEBUG
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE);
#endif

  // Init system clock (48 MHz)
  InitRCC(25, 48, FLASH_ACR_LATENCY_1WS, RCC_CFGR_HPRE_DIV1, RCC_CFGR_PPRE2_DIV2, RCC_CFGR_PPRE1_DIV2, 0);

  //RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN;

  GPIOInit();
  USART2Init();
  DMAInitForUSART2();
}

void uart_log_dma_start(const char *ptr, unsigned int length)
{
  DMA1_Stream6->NDTR = length;
  DMA1_Stream6->M0AR = (unsigned int)ptr;
  DMA_Cmd(DMA1_Stream6, ENABLE);
}

int uart_log_dma_in_progress(void)
{
  int in_progress = DMA1_Stream6->CR & DMA_SxCR_EN;
  //DMA1->HIFCR = 0x0F7D0F7D;
  return in_progress;
}