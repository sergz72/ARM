#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f7xx.h>

#define DEBUG
#ifndef NULL
#define NULL 0
#endif

#define _MAX_DELAY      0xFFFFFFFFU

#define HSI_VALUE 8000000
#define LSE_VALUE 32768
#define PCLK1_FREQ 12000000
#define PCLK2_FREQ 24000000
#define SYSCLK_FREQ 48000000

/* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLL_M) * PLL_N = 288 MHZ */
#define PLL_M      8
#define PLL_N      288
/* USB OTG FS, SDIO and RNG Clock =  PLL_VCO / PLLQ = 48 MHZ */
#define PLL_Q      6
/* SYSCLK = PLL_VCO / PLL_P = 48 MHZ */
#define PLL_P      6

#define FLASH_LATENCY FLASH_ACR_LATENCY_3WS

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

#define USB_MAX_DESCRIPTOR_BUILDERS 1
#define USB_MAX_CONFIGURATION_INTERFACES 1
#define USB_MAX_INTERFACE_ENDPOINTS 2
#define USB_MAX_DEVICES 1

void delay(unsigned int);
void delayms(unsigned int);

#endif
