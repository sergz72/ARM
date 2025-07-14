#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <board_common.h>

#include "stm32h5xx.h"

#define SYSTICK_MULTIPLIER 30
#define SPI_CLOCK 48000000
#define BOARD_PCLK1 240000000

// clr PC13
#define LED_ON GPIOC->BSRR = 0x20000000
// set PC13
#define LED_OFF GPIOC->BSRR = 0x2000

//PB1
#define BUTTON1_PRESSED (GPIOB->IDR & 2)
//PB2
#define BUTTON2_PRESSED (GPIOB->IDR & 4)

//PA7
#define ST7789_DC_PIN_CLR GPIOA->BSRR = 0x800000
#define ST7789_DC_PIN_SET GPIOA->BSRR = 0x80
//PA6
#define ST7789_RST_PIN_CLR GPIOA->BSRR = 0x400000
#define ST7789_RST_PIN_SET GPIOA->BSRR = 0x40
//PB0
#define ST7789_CS_PIN_CLR GPIOB->BSRR = 0x10000
#define ST7789_CS_PIN_SET GPIOB->BSRR = 1

//PA5=SPI1_SCK,PA7=SPI1_MOSI
#define SPI_LCD           SPI3
#define SPI_LCD_ENABLE    RCC->APB1LENR |= RCC_APB1LENR_SPI3EN
#define SPI_TIMEOUT       1000000
#define SPI_LCD_SCK_PIN   GPIO_Pin_1
#define SPI_LCD_SCK_PORT  GPIOA
#define SPI_LCD_SCK_AF    GPIO_AF6_SPI3
#define SPI_LCD_MOSI_PIN  GPIO_Pin_3
#define SPI_LCD_MOSI_PORT GPIOA
#define SPI_LCD_MOSI_AF   GPIO_AF6_SPI3

#define LCD_WIDTH  240
#define LCD_HEIGHT 135
#define ST7789_MADCTL_VALUE (ST7789_MADCTL_MX | ST7789_MADCTL_MV)

#define DAC_REFERENCE_VOLTAGE 3300
#define RAMFUNC __attribute__((section(".RamFunc")))

#define COUNTERS_MAX 366

#define WS2812_MAX_VALUE 0xFF

#include <delay_systick.h>

#ifdef __cplusplus
extern "C" {
#endif

void GatedModeSet(TIM_TypeDef *tim);
void ExternalClockModeSet(TIM_TypeDef *tim);

#ifdef __cplusplus
}
#endif

#endif
