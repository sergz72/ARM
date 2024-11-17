#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define MAX_SHELL_COMMANDS 100
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#include "hardware/i2c.h"

#define I2C_SDA_PIN 6
#define I2C_SCL_PIN 7
#define I2C_TIMEOUT 1000000 // 1s
#define I2C_INST i2c1

#define SSD1306_128_64
#define LCD_ORIENTATION LCD_ORIENTATION_LANDSCAPE

#include <lcd_ssd1306.h>

#define LCD_PRINTF_BUFFER_LENGTH 30
#define DRAW_TEXT_MAX 20
#define USE_VSNPRINTF

#define WS2812_PIN 16

#define KEYBOARD_PIN_START 2
#define KEYBOARD_PIN_END   5

#define OVEN_CONTROL_PIN 8

#define KB_UP 0xE
#define KB_DOWN 0xD
#define KB_START 0xB
#define KB_STOP 0x7


void delayms(unsigned int ms);
void SystemInit(void);
void put_pixel(unsigned int pixel_grb);
unsigned int urgb_u32(unsigned int r, unsigned int g, unsigned int b);
unsigned int get_keyboard_status(void);
void off(void);
void on(void);
void WS2812Init(void);
int get_temperature(void);
void MCP9600Init(void);

#endif
