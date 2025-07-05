#ifndef _BOARD_H
#define _BOARD_H

#include <board_common.h>

#include "stm32g4xx.h"

//#define BOOST_MODE

#ifdef BOOST_MODE
#define BOARD_PCLK1 168000000
#define BOARD_PCLK2 168000000
#define SYSTICK_MULTIPLIER 21
#define PLLN 42
#define COUNTERS_MAX 256
#define I2C_TIMINGS ?
#else
#define BOARD_PCLK1 144000000
#define BOARD_PCLK2 144000000
#define SYSTICK_MULTIPLIER 18
#define PLLN 36
#define COUNTERS_MAX 219
#define I2C_TIMINGS 0x60715075
#endif

// clr PC6
#define LED_OFF GPIOC->BSRR = 0x400000
// set PC6
#define LED_ON GPIOC->BSRR = 0x40

//PC13
#define BUTTON1_PRESSED (GPIOC->IDR & 0x2000)
//PB8
#define BUTTON2_PRESSED (GPIOB->IDR & 0x100)

#define DAC_REFERENCE_VOLTAGE 3300

#define I2C_TIMEOUT 1000000
#define SPI_TIMEOUT 1000000

#define RAMFUNC __attribute__((section(".RamFunc")))

#define DAC3_PRESENT

#endif
