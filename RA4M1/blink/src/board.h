#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <R7FA4M1AB.h>

#define SYSTICK_MULTIPLIER 8

#define STACK_SIZE 0x400
#define HEAP_SIZE  0

#define LED_PORT   0
#define LED_PIN    12

#define LED_TOGGLE R_PORT0->PODR ^= (1 << LED_PIN)

void SystemInit(void);

#endif
