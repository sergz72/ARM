#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32h503xx.h"

#define SYSTICK_MULTIPLIER 6

// clr PC13
#define LED_ON GPIOC->BSRR = 0x20000000
// set PC13
#define LED_OFF GPIOC->BSRR = 0x2000

void delay(unsigned int);
void delayms(unsigned int);

#endif
