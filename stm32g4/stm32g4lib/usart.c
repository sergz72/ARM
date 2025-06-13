#include "board.h"
#include <usart.h>

USART_TypeDef *usart_printf_instance;

void usart_send_char(USART_TypeDef * instance, char c)
{
  while (!(instance->ISR & USART_ISR_TXE_TXFNF))
    ;
  instance->TDR = c;
}

void usart_send(USART_TypeDef *instance, const char *data, int length)
{
  while (length--)
    usart_send_char(instance, *data++);
}
