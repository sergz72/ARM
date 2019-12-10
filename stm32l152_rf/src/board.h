#ifndef _BOARD_H
#define _BOARD_H

#include <stm32l1xx.h>

#define PCLK1_FREQ 2100000
#define PCLK2_FREQ 2100000

#define FALSE 0

#define SYSTICK_MULTIPLIER 2

// clr PA12
#define ST7066_RS_CLR GPIOA->BSRR = 0x10000000
// set PA12
#define ST7066_RS_SET GPIOA->BSRR = 0x1000

// clr PA11
#define ST7066_E_CLR GPIOA->BSRR = 0x8000000
// set PA11
#define ST7066_E_SET GPIOA->BSRR = 0x800

// clr PA4
#define cc1101_CSN_CLR(d) GPIOA->BSRR = 0x100000
// set PA4
#define cc1101_CSN_SET(d) GPIOA->BSRR = 0x10

// PA1
#define cc1101_GD0 (GPIOA->IDR & 2U)
// PA3
#define cc1101_GD2 (GPIOA->IDR & 8U)

#define CC1101_TIMEOUT 0xFFFF

#define DEVICE_ADDRESS 1
#define RF_FREQUENCY 433100
#define RF_MODE CC1101_MODE_GFSK_1200
#define TX_POWER CC1101_TX_POWER_M20_433

#define SENSOR_UPDATE_PERIOD 300

#define VBAT_R1 150
#define VBAT_R2 220

#define COMMUNICATION_COUNTER_MAX 1800 // 30 min

void ST7066_DATA_SET(unsigned int d);
#define ST7066_DELAY st7066delay()
#define ST7066_DELAY_MS(ms) delayms(ms)

void delay(unsigned int);
void delayms(unsigned int);
void st7066delay(void);

#endif
