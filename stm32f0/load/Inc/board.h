#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f0xx.h>

// clr PA11
#define ST7066_RS_CLR GPIOA->BSRR = 0x8000000
// set PA11
#define ST7066_RS_SET GPIOA->BSRR = 0x800

// clr PA12
#define ST7066_E_CLR GPIOA->BSRR = 0x10000000
// set PA12
#define ST7066_E_SET GPIOA->BSRR = 0x1000

// clr PA11
#define ST7066_RS_CLR GPIOA->BSRR = 0x8000000
// set PA11
#define ST7066_RS_SET GPIOA->BSRR = 0x800

// clr PB0
#define LED_RED_OFF GPIOB->BSRR = 0x10000
// set PB0
#define LED_RED_ON GPIOB->BSRR = 1

// clr PB1
#define LED_GREEN_OFF GPIOB->BSRR = 0x20000
// set PB1
#define LED_GREEN_ON GPIOB->BSRR = 2

#define SW_BUTTON_PRESSED !(GPIOA->IDR & 0x80) //PA7
#define ONOFF_BUTTON_PRESSED !(GPIOA->IDR & 0x8000) // PA15
#define DT_STATE (GPIOF->IDR & 2) //PF1

void ST7066_DATA_SET(unsigned int d);
//#define ST7066_DELAY st7066_delay()
#define ST7066_DELAY
#define ST7066_DELAY_MS(ms) HAL_Delay(ms)

void user_init(void);
void user_loop(void);
//void st7066_delay(void);

#endif
