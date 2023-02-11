#ifndef _BOARD_H
#define _BOARD_H

#include <stm32f0xx.h>

#define FALSE 0

#define SYSTICK_MULTIPLIER 8U

#define PCLK1_FREQ 8000000U

#define I2C1_IRQ_ENABLE

// clr PC8
#define LED_BLUE_OFF GPIOC->BSRR = 0x01000000
// set PC8
#define LED_BLUE_ON GPIOC->BSRR = 0x0100

// clr PC9
#define LED_GREEN_OFF GPIOC->BSRR = 0x02000000
// set PC9
#define LED_GREEN_ON GPIOC->BSRR = 0x0200

// set PB4
#define FSY_SET GPIOB->BSRR = 0x0010
// clr PB4
#define FSY_CLR GPIOB->BSRR = 0x00100000

// set PB6
#define CS_SET GPIOB->BSRR = 0x0040
// clr PB6
#define CS_CLR GPIOB->BSRR = 0x00400000

#define DDS_COMMAND_QUEUE_LENGTH 5

void delay(unsigned int);
void delayms(unsigned int);
void TXISHandler(I2C_TypeDef* I2Cx);
void RXNEHandler(I2C_TypeDef* I2Cx);
void STOPHandler(I2C_TypeDef* I2Cx);
void ADDRHandler(I2C_TypeDef* I2Cx);

#endif
