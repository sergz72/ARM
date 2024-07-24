#ifndef BOARD_H
#define BOARD_H

#include <stm32f0xx.h>
#include <gpio.h>

#define BOARD_PCLK 48000000

#define NULL 0

#define SERIAL_CONSOLE 1

#define DRILL_BUTTON_PORT GPIOA
#define DRILL_BUTTON_PIN GPIO_Pin_4
// check PA4
#define DRILL_BUTTON_NOT_PRESSED (GPIOA->IDR & 0x10)
#define DRILL_BUTTON_PRESSED (!(GPIOA->IDR & 0x10))

#define IRON_BUTTON_PORT GPIOB
#define IRON_BUTTON_PIN GPIO_Pin_1
// check PB1
#define IRON_BUTTON_NOT_PRESSED (GPIOB->IDR & 2)
#define IRON_BUTTON_PRESSED (!(GPIOB->IDR & 2))

// PWM Frequency is 10 kHz
#define TIM_PERIOD (BOARD_PCLK / 10000)

void delay(unsigned int us);
//void delay2(unsigned int us);
void char_send(char c);
void str_send(char *s);
void error(void);
void OK(void);
void END(void);

extern int (*serial_handler)(char);
extern char command, *write_buffer_p;

#endif
