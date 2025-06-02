#ifndef BOARD_H
#define BOARD_H

#include "gd32f30x.h"

#ifndef NULL
#define NULL 0
#endif

#define SYSTICK_MULTIPLIER 6

#define LED_PORT  GPIOB
#define LED_PIN   GPIO_PIN_2
#define LED_CLOCK RCU_GPIOB

#define LED_ON gpio_bit_write(LED_PORT, LED_PIN, 1);
#define LED_OFF gpio_bit_write(LED_PORT, LED_PIN, 0);

#define I2C_TIMEOUT 1000000

void delay(unsigned int);
void delayms(unsigned int);
void HALInit(void);

#endif
