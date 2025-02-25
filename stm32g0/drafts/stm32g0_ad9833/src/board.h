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

// PA8
#define SPI2_NSS_CLR GPIOA->BSRR = 0x01000000
#define SPI2_NSS_SET GPIOA->BSRR = 0x0100

// PA11
#define AD9850_RESET_CLR(channel) GPIOA->BSRR = 0x08000000
#define AD9850_RESET_SET(channel) GPIOA->BSRR = 0x0800

#define SPI_SLAVE_DMA

#define SYSTICK_MULTIPLIER 48

#define SPI_TIMEOUT 1000000

#define AD9850_MAX_CHANNELS 1
#define AD9850_FQUD_CLR(channel)
#define AD9850_FQUD_SET(channel)
#define AD9850_DATA_SET(channel, data)
#define AD9850_CLK_SET(channel)
#define AD9850_CLK_CLR(channel)
#define AD9850_DELAY dummy_call()

#define ADF4351_MAX_CHANNELS 1

//#define DDS_TYPE_AD9833
//#define DDS_TYPE_AD9850
#define DDS_TYPE_ADF4351

void delay(unsigned int us);
void dummy_call(void) __attribute__((noinline));

#endif
