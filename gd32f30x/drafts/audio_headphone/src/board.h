#ifndef BOARD_H
#define BOARD_H

#include "gd32f30x.h"

#define SYSTICK_MULTIPLIER 6

#define LED_PORT  GPIOB
#define LED_PIN   GPIO_PIN_2
#define LED_CLOCK RCU_GPIOB

void delay(unsigned int);
void delayms(unsigned int);
void HALInit(void);

#endif
