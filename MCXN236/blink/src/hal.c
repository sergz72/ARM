#include "board.h"

static void ClockInit(void)
{
  // start divider, divide by 1
  SYSCON->SYSTICKCLKDIV[0] = 0;
  // Systick clock -> SYSTICKCLKDIV[0] output
  SYSCON->SYSTICKCLKSEL0 = 0;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
}

static void GPIOInit(void)
{
  // enable clock for GPIO4
  SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL0_PORT4_MASK | SYSCON_AHBCLKCTRL0_GPIO4_MASK;
  LED_GREEN_OFF;
  LED_RED_OFF;
  LED_BLUE_OFF;
  // configure led pins as outputs
  GPIO4->PDDR |= (1 << BOARD_LED_RED_GPIO_PIN) | (1 << BOARD_LED_BLUE_GPIO_PIN) | (1 << BOARD_LED_GREEN_GPIO_PIN);
}

void HalInit(void)
{
  ClockInit();
  GPIOInit();
}

void _init(void)
{

}
