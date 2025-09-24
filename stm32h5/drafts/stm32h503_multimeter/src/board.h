#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#include <stm32h5xx.h>
#include <gpio.h>

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

#define MAX_SHELL_COMMANDS 30
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define PRINTF_BUFFER_LENGTH 200
#define USE_MYVSPRINTF

#define CAPTURE_PIN_CHARGE GPIO_Pin_3
#define CAPTURE_PIN_CX     GPIO_Pin_5
#define CAPTURE_PORT       GPIOA

#define R_CHARGE_PIN    GPIO_Pin_2
#define R_DISCHARGE_PIN GPIO_Pin_6
#define R_PORT          GPIOA

// clr PA2
#define CHARGE_OFF R_PORT->BSRR = R_CHARGE_PIN<<16
// set PA2
#define CHARGE_ON R_PORT->BSRR = R_CHARGE_PIN

#define TIMER_INTERRUPT_PRIORITY 0

extern volatile int capacity_measurement_done;

#ifdef __cplusplus
extern "C" {
#endif

void CDCInit(void);
int getch_(void);
void discharge_off(void);
void discharge_on(void);
void capacity_measurement_start(void);
unsigned int get_capacity_measurement_start_time(void);
unsigned int get_capacity_measurement_end_time(void);

#ifdef __cplusplus
}
#endif

#endif
