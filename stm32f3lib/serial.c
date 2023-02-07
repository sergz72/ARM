#include "board.h"
#include <serial.h>
#include <nvic.h>

serial_read_handler _read_handlers[5];

void usart_handler(USART_TypeDef *USART, int usart_no)
{
  volatile int DR;
  unsigned int SR = USART->ISR;

  if (SR & USART_ISR_ORE) // overrun
  {
    DR = USART->RDR;
	  return;
  }

  if (SR & USART_ISR_RXNE) {                  // read interrupt
    if (_read_handlers[usart_no])
      _read_handlers[usart_no](USART->RDR & 0xFF);
    else
      DR = USART->RDR; // to avoid overrun
  }
}

/*----------------------------------------------------------------------------
  USART1_IRQHandler
  Handles USART1 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART1_IRQHandler (void) {
  usart_handler(USART1, 1);
}

/*----------------------------------------------------------------------------
  USART2_IRQHandler
  Handles USART2 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART2_IRQHandler (void) {
  usart_handler(USART2, 2);
}

/*----------------------------------------------------------------------------
  USART3_IRQHandler
  Handles USART3 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART3_IRQHandler (void) {
  usart_handler(USART3, 3);
}

void SERIAL_INTERRUPT_ENABLE(int uart_no)
{
  switch (uart_no)
  {
    case 1:
      //enable USART1 global interrupt
			NVIC_Init(USART1_IRQn, 0, 0, ENABLE);
      break;
    case 2:
      //enable USART2 global interrupt
			NVIC_Init(USART2_IRQn, 0, 0, ENABLE);
      break;
    case 3:
      //enable USART3 global interrupt
			NVIC_Init(USART3_IRQn, 0, 0, ENABLE);
      break;
  }
}

void SERIAL_INTERRUPT_DISABLE(int uart_no)
{
  switch (uart_no)
  {
    case 1:
      //disable USART1 global interrupt
			NVIC_Init(USART1_IRQn, 0, 0, DISABLE);
      break;
    case 2:
      //disable USART2 global interrupt
			NVIC_Init(USART2_IRQn, 0, 0, DISABLE);
      break;
    case 3:
      //disable USART3 global interrupt
			NVIC_Init(USART3_IRQn, 0, 0, DISABLE);
      break;
  }
}

void serial_init(int uart_no, int uart_speed, serial_read_handler read_handler)
{
  USART_TypeDef *USART;
	
  _read_handlers[uart_no] = read_handler;

//Start setting up UART

  switch (uart_no)
  {
    case 1:
      //enable the USART1 peripheral clock
      RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
      USART = USART1;
      break;
    case 2:
      //enable the USART2 peripheral clock
      RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
      USART = USART2;
      break;
    case 3:
      //enable the USART3 peripheral clock
      RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
      USART = USART3;
      break;
		default:
			return;
  }

  // 1. Enable the USART by writing the UE bit in USART_CR1 register to 1.
  USART->CR1 |= USART_CR1_UE;
  // 2. Program the M bit in USART_CR1 to define the word length.(bit is 0 => 8bits)
  USART->CR1 &= ~USART_CR1_M;
  // 3. Program the number of stop bits in USART_CR2. (bits are 10 => 2 stopbits)
  USART->CR2 &= ~USART_CR2_STOP;
  USART->CR2 |= USART_CR2_STOP_1;
  // 6. Set the RE bit USART_CR1. This enables the receiver which begins searching for a
  //    start bit.
  USART->CR1 |= USART_CR1_RE;
  // enable transmiter
  USART->CR1 |= USART_CR1_TE;
  //enable receive-data interrupt
  USART->CR1 |= USART_CR1_RXNEIE;

  // 5. Select the desired baud rate using the baud rate register USART_BRR
  if (uart_no == 1)
    USART->BRR = BOARD_PCLK2 / uart_speed;
  else
    USART->BRR = BOARD_PCLK1 / uart_speed;

  SERIAL_INTERRUPT_ENABLE(uart_no);
}

void serial_send(int uart_no, char c)
{
  switch (uart_no)
  {
    case 1:
      while (!(USART1->ISR & USART_ISR_TXE));
      USART1->TDR = c;
      break;
    case 2:
      while (!(USART2->ISR & USART_ISR_TXE));
      USART2->TDR = c;
      break;
    case 3:
      while (!(USART3->ISR & USART_ISR_TXE));
      USART3->TDR = c;
      break;
  }
}

void serial_puts(int uart_no, const char *s)
{
  while (*s)
    serial_send(uart_no, *s++);
}
