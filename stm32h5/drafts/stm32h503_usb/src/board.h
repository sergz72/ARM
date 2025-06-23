#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32h5xx.h"

#define SYSTICK_MULTIPLIER 6

// clr PC13
#define LED_ON GPIOC->BSRR = 0x20000000
// set PC13
#define LED_OFF GPIOC->BSRR = 0x2000

//PA0
#define BUTTON_PRESSED (GPIOA->IDR & 1)

#endif
