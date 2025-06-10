#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f4xx.h>

#ifndef NULL
#define NULL 0
#endif

#define HSI_VALUE 8000000
#define HSE_VALUE 8000000

#define HCLK_FREQ  48000000
#define PCLK1_FREQ 24000000
#define PCLK2_FREQ 48000000
#define I2S_CLOCK 10000

#define FALSE 0

#define CS43L22_ADDRESS 0x94
#define I2C_TIMEOUT 0xFFFF
#define CS43L22_RESET_PIN  GPIO_Pin_4
#define CS43L22_RESET_GPIO GPIOD
#define CS43L22_RESET_SET CS43L22_RESET_GPIO->BSRR = CS43L22_RESET_PIN

// clr PD12
#define LED1_ON GPIOD->BSRR = 0x10000000
// set PD12
#define LED1_OFF GPIOD->BSRR = 0x1000
// clr PD13
#define LED2_ON GPIOD->BSRR = 0x20000000
// set PD13
#define LED2_OFF GPIOD->BSRR = 0x2000
// clr PD14
#define LED3_ON GPIOD->BSRR = 0x40000000
// set PD14
#define LED3_OFF GPIOD->BSRR = 0x4000
// clr PD15
#define LED4_ON GPIOD->BSRR = 0x80000000
// set PD15
#define LED4_OFF GPIOD->BSRR = 0x8000

#define SYSTICK_MULTIPLIER 6

void delay(unsigned int);
void delayms(unsigned int);

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 1000
#define SERIAL_QUEUE_SIZE 100

#include <gpio.h>
#include <usb_func.h>
#include <fixed_queue.h>

extern Queue usb_q;

#endif
