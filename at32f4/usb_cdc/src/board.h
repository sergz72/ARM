#ifndef BOARD_H
#define BOARD_H

#include "at32f403a_407.h"

#ifndef NULL
#define NULL 0
#endif

#define SYSTICK_MULTIPLIER 6

#define LED_PIN GPIO_PINS_13
#define LED_PORT GPIOC
#define LED_CLOCK CRM_GPIOC_PERIPH_CLOCK

#endif
