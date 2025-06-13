#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32g431xx.h"

#define SYSTICK_MULTIPLIER 2

// clr PC6
#define LED_OFF GPIOC->BSRR = 0x400000
// set PC6
#define LED_ON GPIOC->BSRR = 0x40

#endif
