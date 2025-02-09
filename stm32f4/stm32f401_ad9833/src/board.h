#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f4xx.h>

#ifndef NULL
#define NULL 0
#endif

#define HSI_VALUE 8000000
#define HSE_VALUE 25000000

#define BOARD_PCLK1 24000000

#define MAX_TRANSFER_SIZE 512

#define WAIT_FOR_INTERRUPT __WFI()

//PC13
#define LED_TIMER_ON GPIOC->BSRR = 0x20000000
#define LED_TIMER_OFF GPIOC->BSRR = 0x2000

//PB10
#define LED_COMMAND_ON GPIOB->BSRR = 0x04000000
#define LED_COMMAND_OFF GPIOB->BSRR = 0x0400

//#define SPI_SLAVE_DMA

#endif
