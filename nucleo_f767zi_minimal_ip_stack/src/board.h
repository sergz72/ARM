#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f7xx.h>

#define DEBUG
#define NULL 0

#define HSI_VALUE 8000000
#define LSE_VALUE 32768
#define PCLK1_FREQ 12000000
#define PCLK2_FREQ 24000000
#define SYSCLK_FREQ 48000000

/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N = 192 MHZ */
#define PLL_M      8
#define PLL_N      192
/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ = 48 MHZ */
#define PLL_Q      4
/* SYSCLK = PLL_VCO / PLL_P = 48 MHZ */
#define PLL_P      4

#define FLASH_LATENCY FLASH_ACR_LATENCY_1WS

#define SYSTICK_MULTIPLIER 6

// clr PB0
#define LD1_OFF GPIOB->BSRR = 0x10000
// set PB0
#define LD1_ON GPIOB->BSRR = 0x1

// clr PB7
#define LD2_OFF GPIOB->BSRR = 0x800000
// set PB7
#define LD2_ON GPIOB->BSRR = 0x80

// clr PB14
#define LD3_OFF GPIOB->BSRR = 0x40000000
// set PB14
#define LD3_ON GPIOB->BSRR = 0x4000

#define ETH_MACMIIAR_CR_Div ETH_MACMIIAR_CR_Div26
/* Definition of the Ethernet driver buffers size and count */
#define ETH_RX_BUF_SIZE                ETH_MAX_PACKET_SIZE /* buffer size for receive               */
#define ETH_TX_BUF_SIZE                ETH_MAX_PACKET_SIZE /* buffer size for transmit              */
#define ETH_RXBUFNB                    4       /* 4 Rx buffers of size ETH_RX_BUF_SIZE  */
#define ETH_TXBUFNB                    4       /* 4 Tx buffers of size ETH_TX_BUF_SIZE  */
#define MAC_ADDR0   2
#define MAC_ADDR1   0
#define MAC_ADDR2   0
#define MAC_ADDR3   0
#define MAC_ADDR4   0
#define MAC_ADDR5   0

#define ETH_IRQ_QUEUE_SIZE 4
#define ETH_USER_QUEUE_SIZE 4

void delay(unsigned int);
void delayms(unsigned int);

#define ETH_DEBUG(name) UART_Printf(USART3, "%s\r\n", name)
#define ETH_DEBUG1(name, param) UART_Printf(USART3, "%s %d\r\n", name, param)
#define ETH_DEBUG2(name, param1, param2) UART_Printf(USART3, "%s %d %d\r\n", name, param1, param2)
#define ETH_DEBUG3(name, param1, param2, param3) UART_Printf(USART3, "%s %d %d %d\r\n", name, param1, param2, param3)
#define ETH_DEBUG4(name, param1, param2, param3, param4) UART_Printf(USART3, "%s %d %d %d %d\r\n", name, param1, param2, param3, param4)
#define ETH_DEBUG5(name, param1, param2, param3, param4, param5) UART_Printf(USART3, "%s %d %d %d %d %d\r\n", name, param1, param2, param3, param4, param5)

#include <uart.h>

#endif
