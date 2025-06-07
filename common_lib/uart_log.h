#ifndef UART_LOG_H
#define UART_LOG_H

void uart_log_init(void);
int uart_log_process(void);

void uart_log_printf0(const char *message);
void uart_log_printf1(const char *format, unsigned int parameter1);
void uart_log_printf2(const char *format, unsigned int parameter1, unsigned int parameter2);
void uart_log_printf3(const char *format, unsigned int parameter1, unsigned int parameter2, unsigned int parameter3);
void uart_log_printf4(const char *format, unsigned int parameter1, unsigned int parameter2, unsigned int parameter3, unsigned int parameter4);

int uart_log_dma_in_progress(void); // should be defined in hal.c
void uart_log_dma_start(const char *ptr, unsigned int length); // should be defined in hal.c

#endif
