#ifndef _BOARD_H
#define _BOARD_H

#include <stm32g0xx.h>

#ifndef NULL
#define NULL 0
#endif

#define PLLM 2
#define PLLN 12
#define PLLR 2
#define BOARD_PCLK2 48000000

#define MAX_TRANSFER_SIZE 512

#define WAIT_FOR_INTERRUPT __WFI()

//PA1
#define LED_TIMER_ON GPIOA->BSRR = 0x20000
#define LED_TIMER_OFF GPIOA->BSRR = 2

//PA2
#define LED_COMMAND_ON GPIOA->BSRR = 0x40000
#define LED_COMMAND_OFF GPIOA->BSRR = 4

//PA11
#define INTERRUPT_FLAG_CLR GPIOA->BSRR = 0x08000000
#define INTERRUPT_FLAG_SET GPIOA->BSRR = 0x0800

#define SPI_SLAVE_DMA

#endif
