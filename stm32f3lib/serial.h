#ifndef SERIAL_H_
#define SERIAL_H_

typedef void (*serial_read_handler)(char);

void serial_init(int uart_no, int uart_speed, serial_read_handler read_handler);
void serial_send(int uart_no, char c);
void SERIAL_INTERRUPT_ENABLE(int uart_no);
void SERIAL_INTERRUPT_DISABLE(int uart_no);
void serial_puts(int uart_no, const char *s);

#endif /*SERIAL_H_*/
