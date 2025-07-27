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
#define LCD_DC_PIN_CLR GPIOA->BSRR = 0x800000
#define LCD_DC_PIN_SET GPIOA->BSRR = 0x80
//PA6
#define LCD_RST_PIN_CLR GPIOA->BSRR = 0x400000
#define LCD_RST_PIN_SET GPIOA->BSRR = 0x40
//PB0
#define LCD_CS_PIN_CLR GPIOB->BSRR = 0x10000
#define LCD_CS_PIN_SET GPIOB->BSRR = 1

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

#define SSD1331

#ifdef ST7789
#define LCD_WIDTH  240
#define LCD_HEIGHT 135
#define ST7789_MADCTL_VALUE (ST7789_MADCTL_MX | ST7789_MADCTL_MV)
#define DISPLAY_MAX_COLUMNS    13
#define DISPLAY_MAX_ROWS       6
#endif

#ifdef SSD1357
#define LCD_WIDTH  64
#define LCD_HEIGHT 64
#define SSD1357_MADCTL_VALUE (SSD1357_MADCTL_X_MIRROR | SSD1357_MADCTL_Y_MIRROR)
#define DISPLAY_MAX_COLUMNS    8
#define DISPLAY_MAX_ROWS       8
#include <font8.h>
#endif

#ifdef SSD1331
#define LCD_WIDTH  96
#define LCD_HEIGHT 64
#define SSD1331_MADCTL_VALUE (SSD1331_MADCTL_X_MIRROR | SSD1331_MADCTL_Y_MIRROR)
#define DISPLAY_MAX_COLUMNS    12
#define DISPLAY_MAX_ROWS       8
#include <font8.h>
#endif

#define DAC_REFERENCE_VOLTAGE 3300
#define RAMFUNC __attribute__((section(".RamFunc")))

#define COUNTERS_MAX 366

#define WS2812_MAX_VALUE 0xFF

#define WEAK __attribute__((weak))

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
