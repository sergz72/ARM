#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include "stm32g4xx.h"

//#define BOOST_MODE

#ifdef BOOST_MODE
#define BOARD_PCLK1 168000000
#define BOARD_PCLK2 168000000
#define SYSTICK_MULTIPLIER 21
#define PLLN 42
#define COUNTERS_MAX 256
#define I2C_TIMINGS ?
#else
#define BOARD_PCLK1 144000000
#define BOARD_PCLK2 144000000
#define SYSTICK_MULTIPLIER 18
#define PLLN 36
#define COUNTERS_MAX 219
#define I2C_TIMINGS 0x60715075
#endif

#define USART_BAUD_RATE 115200
#define USART_BUFFER_SIZE 1024

// clr PC6
#define LED_OFF GPIOC->BSRR = 0x400000
// set PC6
#define LED_ON GPIOC->BSRR = 0x40

//PC13
#define BUTTON1_PRESSED (GPIOC->IDR & 0x2000)
//PB8
#define BUTTON2_PRESSED (GPIOB->IDR & 0x100)

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 200

#define DAC_REFERENCE_VOLTAGE 3300
#define DEFAULT_DAC1_VOLTAGE  400
#define DEFAULT_DAC3_VOLTAGE  2500

#define TIMER_EVENT_FREQUENCY 10

#define USART_INTERRUPT_PRIORITY 2
#define COUNTERS_INTERRUPT_PRIORITY 1
#define TIMER_INTERRUPT_PRIORITY 0

extern unsigned int counter_low, counter_high, counter_freq_low, counter_freq_high;

unsigned int mv_to_12(unsigned int mv);
void pwm_set_frequency_and_duty(unsigned int frequency, unsigned int duty);
void stop_counters(void);
void start_counters(void);

#define SSD1306_128_32
#define LCD_ORIENTATION LCD_ORIENTATION_LANDSCAPE

#define LCD_PRINTF_BUFFER_LENGTH 30
#define DRAW_TEXT_MAX 20
#define USE_MYVSPRINTF

#define I2C_TIMEOUT 1000000
#define SPI_TIMEOUT 1000000

#define WS2812_MAX_LEDS 4
#define WS2812_MAX_VALUE 0x40

#endif
