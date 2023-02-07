#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f4xx.h>

#ifndef NULL
#define NULL 0
#endif

#define HSI_VALUE 8000000
#define HSE_VALUE 25000000

//#define PCLK1_FREQ 24000000
//#define PCLK2_FREQ 24000000

#define FALSE 0

// clr PC13
#define LED_ON GPIOC->BSRR = 0x20000000
// set PC13
#define LED_OFF GPIOC->BSRR = 0x2000

#define SYSTICK_MULTIPLIER 6

#define LCD_HWWIDTH 128
#define LCD_HWHEIGHT 64
#define LCD_PRINTF_BUFFER_LENGTH 40
#define USE_VSNPRINTF
#define LCD_WAIT 5
#define LCD_DELAY 50
#define LCD_ORIENTATION LCD_ORIENTATION_LANDSCAPE

#define LCD_DI  1
#define LCD_RW  2
#define LCD_E   4
#define LCD_CS1 8
#define LCD_CS2 16
#define LCD_RST 32

#define LCD_DATA_SET(a) GPIOA->ODR = (GPIOA->ODR & 0xFF00) | (a & 0xFF);
#define LCD_DATA_OFF
#define LCD_INIT lcd_init()

#define DRAW_TEXT_MAX 20

#define I2C_SOFT
#define i2c_dly delay(5) // 100 khz

#define SI5351_XTAL_FREQ					25000000
#define SI5351_CHANNELS 4

#define SPI3_DIO_SET GPIOB->BSRR = 0x100
#define SPI3_DIO_CLR GPIOB->BSRR = 0x1000000
#define SPI3_CLK_SET GPIOA->BSRR = 0x8000
#define SPI3_CLK_CLR GPIOA->BSRR = 0x80000000
#define SPI3_CHECK_DIO (GPIOB->IDR & 0x100)
#define SPI3_CS_SET GPIOB->BSRR = 0x20
#define SPI3_CS_CLR GPIOB->BSRR = 0x200000
#define SPI3_DELAY 50
#define SPI3_LSB_FIRST

void delay(unsigned int);
void delayms(unsigned int);
void LCD_CONTROL_SET(int);
void LCD_CONTROL_CLR(int);
void lcd_init(void);
int I2CCheck(int idx, int device_id);
void SCL_HIGH(int);
void SCL_LOW(int);
void SDA_HIGH(int);
void SDA_LOW(int);
int SDA_IN(int);

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 100
#define SERIAL_QUEUE_SIZE 100

#include <gpio.h>
#include <usb_func.h>
#include <lcd_ks0108_buffered.h>
#include "keyboard_tm1638.h"
#include <fixed_queue.h>

extern int keyboard_present;
extern Queue usb_q;

#endif
