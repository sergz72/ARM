#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f4xx.h>

#ifndef NULL
#define NULL 0
#endif

#define HSI_VALUE 8000000
#define HSE_VALUE 25000000

#define HCLK_FREQ 48000000
#define PCLK1_FREQ 24000000
#define PCLK2_FREQ 24000000

#define FALSE 0

// clr PC13
#define LED_ON GPIOC->BSRR = 0x20000000
// set PC13
#define LED_OFF GPIOC->BSRR = 0x2000

#define SYSTICK_MULTIPLIER 6

// set PB7
#define TLV_RESET_RELEASE GPIOB->BSRR = 0x80

void delay(unsigned int);
void delayms(unsigned int);

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 100
#define SERIAL_QUEUE_SIZE 100

#define I2C_TIMEOUT HCLK_FREQ

#include <fixed_queue.h>
#include "usb_func.h"

extern Queue usb_q;

#endif
