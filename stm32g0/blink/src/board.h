#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32g031xx.h"

#define SYSTICK_MULTIPLIER 2

// clr PA4
#define LED_OFF GPIOA->BSRR = 0x100000
// set PA4
#define LED_ON GPIOA->BSRR = 0x10

void delay(unsigned int);
void delayms(unsigned int);

#endif
