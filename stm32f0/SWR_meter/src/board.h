#ifndef BOARD_H
#define BOARD_H

#include <stm32f0xx.h>

#define BOARD_PCLK 8000000

#define I2C_TIMING 0x00210507

#define SSD1306_128_64

#include <lcd_ssd1306.h>

#define SSD1306_SWITCHCAPVCC

#define NULL 0
#define FALSE 0
#define TRUE 1

#define vsprintf_s(a, b, c, d) vsprintf(a, c, d)

#define VREF 330
#define FWD_COEF 174
#define FWD_DIV 10
#define VCC_COEF 514
#define VCC_DIV 1000
#define RL 50
#define PWR_COEF 100
#define SWR_MAX 9999
#define SWR_MAX_BAR 1000
#define PWR_MAX_BAR 1000

#endif
