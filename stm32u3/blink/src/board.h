#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32u3xx.h"

#define SYSTICK_MULTIPLIER 12
#define SYSTICK_DIVIDER    8

// clr PA5
#define LED_OFF GPIOA->BSRR = 0x200000
// set PA5
#define LED_ON GPIOA->BSRR = 0x20

#endif
