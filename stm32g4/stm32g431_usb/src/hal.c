#include "board.h"
#include <gpio.h>
#include <nvic.h>
#include <systick.h>

static void GPIOInit(void)
{
  //LED
  GPIO_Init(GPIOC,
            GPIO_Pin_6,
            GPIO_Mode_OUT,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

static void USB_Init(void)
{
  RCC->CCIPR |= RCC_CCIPR_CLK48SEL_1; // pll1_q_ck selected as usb kernel clock
  RCC->APB1ENR1 |= RCC_APB1ENR1_USBEN;

  NVIC_Init(USB_LP_IRQn, 1, 3, ENABLE);
}

// HSE = 8MHz
static void ClockInit(void)
{
  RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;

  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;

#ifdef BOOST_MODE
  // boost mode on
  PWR->CR5 = 0;
#endif

  //The regulator is ready in the selected voltage range
  while (PWR->SR2 & (PWR_SR2_REGLPF | PWR_SR2_VOSF))
    ;

  // 4 wait states
  unsigned int temp = FLASH->ACR & ~FLASH_ACR_LATENCY;
  FLASH->ACR = temp | FLASH_WAIT_STATES;

  // HSE clock selected as PLL clock entry
  // PLLM = 1
  // PLLR = 2
  // PLLQ = 2
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE | (PLLN << RCC_PLLCFGR_PLLN_Pos) | RCC_PLLCFGR_PLLREN | RCC_PLLCFGR_PLLQEN;

  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;
}

void SystemInit(void)
{
  ClockInit();

  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOAEN;// | RCC_AHB2ENR_GPIOBEN;

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  //USB Type-C and Power Delivery Dead Battery disable.
  PWR->CR3 |= PWR_CR3_UCPD_DBDIS;

  GPIOInit();
  USB_Init();
}

void _init(void)
{

}
