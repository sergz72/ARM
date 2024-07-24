#ifndef BOARD_H
#define BOARD_H

/*!< STM32F10X_LD: STM32 Low density devices */
#define USB_ENABLE
#include <stm32f10x.h>
#include <usb_cdc_.h>

#define PLL_SOURCE_HSE        // HSE (8MHz) used to clock the PLL, and the PLL is used as system clock source

/// Master clock frequency, in Hz.
#define BOARD_MCK               72000000

#define CMD_BUF_SIZE 10
#define RESPONSE_BUFFER_SIZE 100

#define USB_UP_LED_ON
#define USB_UP_LED_OFF

// clr PA9
#define USB_CONNECT GPIOA->BSRR = 0x2000000
// set PA9
#define USB_DISCONNECT GPIOA->BSRR = 0x200

#define TEMP_SENSOR_ADC_CHANNEL 10

// check PA10
#define VBUS (GPIOA->IDR & 0x400)

#define _PWM_PERIOD_ 3600 // 20 khz
#define _TIM1_PERIOD_ 180 //
#define _TIM1_PRESCALER_ 1 //

#define MAX_CHANNEL 3

#define RL 75 // low side resistor
#define RH00 75 // high size resisror 00
#define RH01 75 // high size resisror 01
#define RH10 217 // high size resisror 10
#define RH11 217 // high size resisror 11
#define RH20 293 // high size resisror 20
#define RH21 293 // high size resisror 21
#define RI 2150 // current resistor, mOhm

#define VREF 3300

typedef struct {
	int U, I;
} UI;

typedef struct {
	UI set, fact;
	int duty;
} Channel;

typedef struct {
	Channel channel[MAX_CHANNEL];
	int temp;
} Status;

void delay(unsigned int);
void systick_start(unsigned int us);
void systick_stop(void);
void systick_wait(void);
void pwm_init(void);
void process_command(char *command, char *buf, unsigned int command_size);
void setDuty(int channel, int duty);
extern int Sys_Tick;
//extern unsigned int TIM_PERIOD;
extern unsigned int command_size;
extern Status status;

#endif
