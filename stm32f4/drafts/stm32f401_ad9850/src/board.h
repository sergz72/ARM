#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f4xx.h>

#ifndef NULL
#define NULL 0
#endif

#define HSI_VALUE 8000000
#define HSE_VALUE 25000000

#define HCLK_FREQ 48000000

#define FALSE 0

// clr PC13
#define LED_ON GPIOC->BSRR = 0x20000000
// set PC13
#define LED_OFF GPIOC->BSRR = 0x2000

#define SYSTICK_MULTIPLIER 6

void delay(unsigned int);
void delayms(unsigned int);

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 100
#define SERIAL_QUEUE_SIZE 100

#define AD9850_MCLK_FREQ 125000000
#define AD9850_MAX_CHANNELS 1
#define AD9850_DATA_SET(ch,d) GPIOA->ODR = d
// set PB7
#define AD9850_RESET_SET(ch) GPIOB->BSRR = 0x080
// set PB7
#define AD9850_RESET_CLR(ch) GPIOB->BSRR = 0x0800000
// set PB8
#define AD9850_CLK_SET(ch) GPIOB->BSRR = 0x0100
// set PB8
#define AD9850_CLK_CLR(ch) GPIOB->BSRR = 0x01000000
// set PB9
#define AD9850_FQUD_SET(ch) GPIOB->BSRR = 0x0200
// clr PB9
#define AD9850_FQUD_CLR(ch) GPIOB->BSRR = 0x02000000
#define AD9850_DELAY delay(1)

#include <gpio.h>
#include <usb_func.h>
#include <fixed_queue.h>

extern Queue usb_q;

#endif
