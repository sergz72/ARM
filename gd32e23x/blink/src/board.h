#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <gd32e23x.h>

#define LED_CLOCK RCU_GPIOC
#define LED_PORT GPIOC
#define LED_PIN GPIO_PIN_13
#define LED_TIMER_ON gpio_bit_set(LED_PORT,LED_PIN)
#define LED_TIMER_OFF gpio_bit_reset(LED_PORT,LED_PIN)

#define SYSTICK_MULTIPLIER 1

void SysInit(void);

#endif
