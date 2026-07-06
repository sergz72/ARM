#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32u575xx.h"

#define SYSTICK_MULTIPLIER 6

// clr PC7
#define LED_GREEN_OFF GPIOC->BSRR = 0x800000
// set PC7
#define LED_GREEN_ON GPIOC->BSRR = 0x80

// clr PB7
#define LED_BLUE_OFF GPIOB->BSRR = 0x800000
// set PB7
#define LED_BLUE_ON GPIOB->BSRR = 0x80

// clr PG2
#define LED_RED_OFF GPIOG->BSRR = 0x40000
// set PG2
#define LED_RED_ON GPIOG->BSRR = 4

#endif
