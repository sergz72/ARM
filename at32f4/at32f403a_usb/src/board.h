#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "at32f403a_407.h"

#define SYSTICK_MULTIPLIER 6

#define LED_PIN GPIO_PINS_13
#define LED_PORT GPIOC
#define LED_CLOCK CRM_GPIOC_PERIPH_CLOCK

#define LED_OFF LED_PORT->scr = LED_PIN
#define LED_ON LED_PORT->clr = LED_PIN

#ifdef __cplusplus
extern "C" {
#endif

void HALInit(void);

#ifdef __cplusplus
}
#endif

#endif
