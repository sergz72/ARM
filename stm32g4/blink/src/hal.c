#include "board.h"
#include <systick.h>
#include <gpio.h>

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

void SystemInit(void)
{
  /* 16MHz / 8 => 2M counts per second */
  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);

  //enable the GPIO clock for port GPIOA
  //RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

  //enable the GPIO clock for port GPIOB
  //RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

  //enable the GPIO clock for port GPIOC
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

  GPIOInit();
}

void _init(void)
{
}
