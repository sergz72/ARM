#include "board.h"
#include <systick.h>
#include <gpio.h>
#include <nvic.h>

static void GPIOInit(void)
{
	GPIO_InitTypeDef init;
	
	init.Pin = GPIO_Pin_13;
	init.Mode = GPIO_MODE_INPUT;
	init.Speed = GPIO_SPEED_FREQ_LOW;
	init.Pull = GPIO_NOPULL;
  // Button
  GPIO_Init(GPIOC, &init);

  /// LD1
	init.Pin = GPIO_Pin_0;
	init.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Init(GPIOB, &init);

  /// LD2
	init.Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &init);

  /// LD3
	init.Pin = GPIO_Pin_14;
  GPIO_Init(GPIOB, &init);
}

static void ETHInit(void)
{
  GPIO_InitTypeDef init;

  /*
        RMII_REF_CLK ----------------------> PA1
        RMII_MDIO -------------------------> PA2
        RMII_MDC --------------------------> PC1
        RMII_MII_CRS_DV -------------------> PA7
        RMII_MII_RXD0 ---------------------> PC4
        RMII_MII_RXD1 ---------------------> PC5
        RMII_MII_RXER ---------------------> PG2
        RMII_MII_TX_EN --------------------> PG11
        RMII_MII_TXD0 ---------------------> PG13
        RMII_MII_TXD1 ---------------------> PB13
  */

  init.Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  init.Alternate = GPIO_AF11_ETH;
  GPIO_Init(GPIOA, &init);

  init.Pin = GPIO_Pin_13;
  GPIO_Init(GPIOB, &init);

  init.Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOC, &init);

  init.Pin = GPIO_Pin_2 | GPIO_Pin_11 | GPIO_Pin_13;
  GPIO_Init(GPIOG, &init);

  RCC->AHB1ENR |= RCC_AHB1ENR_ETHMACEN | RCC_AHB1ENR_ETHMACTXEN | RCC_AHB1ENR_ETHMACRXEN;

  /* Set ETH Interrupt priority */
  NVIC_Init(ETH_IRQn, 2, 0, ENABLE);
}

static void USART3Init(void)
{
  GPIO_InitTypeDef init;

  /* Configure DM DP Pins */
  init.Pin = (GPIO_Pin_8 | GPIO_Pin_9);
  init.Mode = GPIO_MODE_AF_PP;
  init.Pull = GPIO_NOPULL;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Alternate = GPIO_AF7_USART3;
  GPIO_Init(GPIOD, &init);

  /* Enable USART3 Clock */
  RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
}

void SystemInit(void)
{
  SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */

  RCC->CR |= RCC_CR_HSEBYP;
  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY));

	/* Select regulator voltage output Scale 3 mode */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	//when VOS[1:0] = '0x01', the maximum value of fHCLK is 144 MHz.
	PWR->CR1 |= PWR_CR1_VOS_0;
	PWR->CR1 &= ~PWR_CR1_VOS_1;

	/* HCLK = SYSCLK */
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	/* PCLK2 = HCLK / 2 */
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
	
	/* PCLK1 = HCLK / 4 */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

	/* Configure the main PLL */
	RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
								 (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);

	/* Enable the main PLL */
	RCC->CR |= RCC_CR_PLLON;

	/* Wait till the main PLL is ready */
	while((RCC->CR & RCC_CR_PLLRDY) == 0);

	/* Configure Flash prefetch, ART Accelerator and wait state */
	FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ARTEN | FLASH_LATENCY;

	/* Select the main PLL as system clock source */
	RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	/* Wait till the main PLL is used as system clock source */
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);

	//When using the ART accelerator, the CPU can read an instruction in just 1 clock from the internal Flash memory (like 0-wait state).
	// So I-cache cannot be used for the internal Flash memory.
  /* Enable I-Cache */
  //SCB_InvalidateICache();
//  SCB_EnableICache();

  /* Enable D-Cache */
  // After reset, the user must invalidate each cache before enabling it, otherwise an UNPREDICTIBLE behaviorcan occur.
  SCB_InvalidateDCache();
  SCB_EnableDCache();

  SCB->VTOR = FLASH_BASE; /* Vector Table Relocation in Internal FLASH */

  /* 48MHz / 8 => 6M counts per second */
  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);

  // for ETHERNET
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  //enable the GPIO clock for port GPIOA
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  //enable the GPIO clock for port GPIOB
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

  //enable the GPIO clock for port GPIOC
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

  //enable the GPIO clock for port GPIOD
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

  //enable the GPIO clock for port GPIOE
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

  //enable the GPIO clock for port GPIOF
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;

  //enable the GPIO clock for port GPIOG
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN;

  GPIOInit();
  USART3Init();
  ETHInit();
}
