#include "board.h"
#include <systick.h>
#include <gpio.h>
#include <nvic.h>
#include <string.h>
#include <usart.h>

static void GPIOInit(void)
{
  //LED
  GPIO_Init(GPIOC,
            GPIO_Pin_6,
            GPIO_Mode_OUT,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_NOPULL
  );
}

/*
 * PA2=USART2_TX
 * PA3=USART2_RX
 */

static void USART2Init(void)
{
  RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);
  GPIO_Init(GPIOA,
            GPIO_Pin_2 | GPIO_Pin_3,
            GPIO_Mode_AF,
            GPIO_Speed_Low,
            GPIO_OType_PP,
            GPIO_PuPd_UP
  );

  USART2->BRR = BOARD_PCLK1 / USART_BAUD_RATE;
  //USART2->CR2 = 0;
  //USART2->CR3 = USART_CR3_DMAT;
  // Transmitter enable, Receiver enable, FIFO enable, RXFIFO not empty interrupt enable
  USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_FIFOEN | USART_CR1_RXNEIE;
  // USART enable in low-power mode, USART enable
  USART2->CR1 |= USART_CR1_UE | USART_CR1_UESM;

  NVIC_Init(USART2_IRQn, 1, 3, 1);
}

void SystemInit(void)
{
  /* 16MHz / 8 => 2M counts per second */
  systick_set_clocksource(STK_CTRL_CLKSOURCE_AHB_DIV8);

  //enable the GPIO clock for port GPIOA
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

  //enable the GPIO clock for port GPIOB
  //RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

  //enable the GPIO clock for port GPIOC
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

  GPIOInit();
  USART2Init();
}

void _init(void)
{
}

void puts_(const char *s)
{
  usart_send(USART2, s, strlen(s));
}
