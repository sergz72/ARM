#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <R7FA4M1AB.h>

#define HOCO_FREQUENCY 24000000
#define PCKD_FREQUENCY (HOCO_FREQUENCY/64)

#define SYSTICK_MULTIPLIER 24

#define STACK_SIZE 0x400
#define HEAP_SIZE  0

#define LED_PORT   0
#define LED_PIN    12

#define LED_TOGGLE R_PORT0->PODR ^= (1 << LED_PIN)

#define TIMER_INTERRUPT_FREQUENCY 1

extern volatile bool timer_interrupt;

void SystemInit(void);

#endif
