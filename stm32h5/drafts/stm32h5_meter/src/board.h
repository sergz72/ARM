#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32h5xx.h"

#define SYSTICK_MULTIPLIER 6

// clr PC13
#define LED_ON GPIOC->BSRR = 0x20000000
// set PC13
#define LED_OFF GPIOC->BSRR = 0x2000

#define USB_MAX_DEVICE_CONFIGURATIONS 1
#define USB_MAX_CONFIGURATIUON_INTERFACES 1
#define USB_MAX_INTERFACE_ENDPOINTS 2

void delay(unsigned int);
void delayms(unsigned int);

#endif
