#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32g4xx.h"

#define SYSTICK_MULTIPLIER 6

// PLL VCO = 96 MHz
#define PLLN 12
#define FLASH_WAIT_STATES 1 // up to 60 MHz
#define BOARD_PCLK1 48000000
#define BOARD_PCLK2 48000000

// clr PC6
#define LED_OFF GPIOC->BSRR = 0x400000
// set PC6
#define LED_ON GPIOC->BSRR = 0x40

#endif
