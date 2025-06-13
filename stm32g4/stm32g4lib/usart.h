#ifndef USART_H
#define USART_H

void usart_send(USART_TypeDef *instance, const char *data, int length);
void usart_send_char(USART_TypeDef * instance, char c);

#endif
