#include "board.h"
#include <gpio.h>
#include <systick.h>
#include <timer.h>
#include <pll.h>

static void USART2Init(void)
{
  //USART2 TX and RX
  GPIO_Init(GPIOB,
            GPIO_Pin_3 | GPIO_Pin_4,
            GPIO_Mode_AF,
            GPIO_Speed_50MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_7);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_7);
}

static void SPIInit(void)
{
  SPI_CS_SET(SPI_CHANNEL_AD);
  SPI_CS_SET(SPI_CHANNEL_MCP);
  GPIO_Init(GPIOB,
            GPIO_Pin_5 | GPIO_Pin_6,
            GPIO_Mode_OUT,
            GPIO_Speed_50MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
  GPIO_Init(GPIOA,
            GPIO_Pin_12 | GPIO_Pin_15,
            GPIO_Mode_OUT,
            GPIO_Speed_50MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

/**
  * @brief  Setup the microcontroller system
  * @param  None
  * @retval None
  */
void SystemInit(void)
{
	/* Set floating point coprosessor access mode. */
  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */

  // system clock = 8 MHz
//  InitRCC(RCC_CFGR_HPRE_DIV1, RCC_CFGR_PPRE1_DIV1, RCC_CFGR_PPRE2_DIV1);
  // system clock = pllin x 3 / 2 (12 MHz)
  InitRCC(3, RCC_CFGR_HPRE_DIV2, RCC_CFGR_PPRE1_DIV1, RCC_CFGR_PPRE2_DIV1);

  /* Enable the SYSCFG module clock */
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  //enable the GPIO clock for port GPIOA
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  //enable the GPIO clock for port GPIOB
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  //LEDS
  GPIO_Init(GPIOB,
            GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9,
            GPIO_Mode_OUT,
            GPIO_Speed_2MHz,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
           );

  /* 12M counts per second */
  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB);

  USART2Init();
  SPIInit();
}
