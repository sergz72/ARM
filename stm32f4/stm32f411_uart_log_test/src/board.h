#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f4xx.h>

#ifndef NULL
#define NULL 0
#endif

#define HSI_VALUE 8000000
#define HSE_VALUE 25000000

#define BOARD_PCLK1 24000000

//PC13
#define LED_ON GPIOC->BSRR = 0x20000000
#define LED_OFF GPIOC->BSRR = 0x2000

#define SYSTICK_MULTIPLIER 6

#define UART_LOG_BUFFER_SIZE 1024
#define UART_LOG_PRINTF_BUFFER_SIZE 1024
#define USART_BAUD_RATE 115200

#define DISABLE_INTERRUPTS __disable_irq()
#define ENABLE_INTERRUPTS __enable_irq()

#endif
