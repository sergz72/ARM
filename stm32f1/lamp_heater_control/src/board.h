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

// clr PC13
#define USB_UP_LED_ON GPIOC->BSRR = 0x20000000
// set PC13
#define USB_UP_LED_OFF GPIOC->BSRR = 0x2000

#define USB_CONNECT
#define USB_DISCONNECT

// crl PA2
#define HEATER0_ON GPIOA->BSRR = 0x40000
// set PA2
#define HEATER0_OFF GPIOA->BSRR = 0x4

// crl PA3
#define HEATER1_OFF GPIOA->BSRR = 0x80000
// set PA3
#define HEATER1_ON GPIOA->BSRR = 0x8

#define TEMP_SENSOR_ADC_CHANNEL 0

//void delay(unsigned int);
//void systick_start(unsigned int us);
//void systick_stop(void);
//void systick_wait(void);
void process_command(char *command, char *buf, unsigned int command_size);
//extern int Sys_Tick;
extern unsigned int command_size;

#endif
