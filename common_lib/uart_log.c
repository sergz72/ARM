#include "board.h"
#include <uart_log.h>
#include <string.h>
#include <stdio.h>

static unsigned int log_buffer[UART_LOG_BUFFER_SIZE];
static unsigned char printf_buffer[UART_LOG_PRINTF_BUFFER_SIZE];
static unsigned int *log_buffer_write_p, *log_buffer_read_p;

void uart_log_init(void)
{
  log_buffer_write_p = log_buffer_read_p = log_buffer;
}

static unsigned int log_buffer_read(void)
{
  unsigned int value = *log_buffer_read_p++;
  if (log_buffer_read_p == log_buffer + UART_LOG_BUFFER_SIZE)
    log_buffer_read_p = log_buffer;
  return value;
}

int uart_log_process(void)
{
  if (!uart_log_dma_in_progress() && log_buffer_write_p != log_buffer_read_p)
  {
    unsigned int l = log_buffer_read();
    const char *format = (const char *)log_buffer_read();
    const char *ptr = l ? (const char *)printf_buffer : format;
    switch (l)
    {
      case 0:
        l = strlen(format);
        break;
      case 1:
        l = snprintf(printf_buffer, sizeof(printf_buffer), format, log_buffer_read());
        break;
      case 2:
        l = snprintf(printf_buffer, sizeof(printf_buffer), format, log_buffer_read(), log_buffer_read());
        break;
      case 3:
        l = snprintf(printf_buffer, sizeof(printf_buffer), format, log_buffer_read(), log_buffer_read(), log_buffer_read());
        break;
      case 4:
        l = snprintf(printf_buffer, sizeof(printf_buffer), format, log_buffer_read(), log_buffer_read(), log_buffer_read(), log_buffer_read());
        break;
      default:
        while (1)
          ;
    }
    uart_log_dma_start(ptr, l);
    return 1;
  }
  return 0;
}

static void log_buffer_add(unsigned int value)
{
  *log_buffer_write_p++ = value;
  if (log_buffer_write_p == log_buffer + UART_LOG_BUFFER_SIZE)
    log_buffer_write_p = log_buffer;
}

void uart_log_printf0(const char *message)
{
  DISABLE_INTERRUPTS;
  log_buffer_add(0);
  log_buffer_add((unsigned int)message);
  ENABLE_INTERRUPTS;
}

void uart_log_printf1(const char *format, unsigned int parameter1)
{
  DISABLE_INTERRUPTS;
  log_buffer_add(1);
  log_buffer_add((unsigned int)format);
  log_buffer_add(parameter1);
  ENABLE_INTERRUPTS;
}

void uart_log_printf2(const char *format, unsigned int parameter1, unsigned int parameter2)
{
  DISABLE_INTERRUPTS;
  log_buffer_add(2);
  log_buffer_add((unsigned int)format);
  log_buffer_add(parameter1);
  log_buffer_add(parameter2);
  ENABLE_INTERRUPTS;
}

void uart_log_printf3(const char *format, unsigned int parameter1, unsigned int parameter2, unsigned int parameter3)
{
  DISABLE_INTERRUPTS;
  log_buffer_add(3);
  log_buffer_add((unsigned int)format);
  log_buffer_add(parameter1);
  log_buffer_add(parameter2);
  log_buffer_add(parameter3);
  ENABLE_INTERRUPTS;
}

void uart_log_printf4(const char *format, unsigned int parameter1, unsigned int parameter2, unsigned int parameter3, unsigned int parameter4)
{
  DISABLE_INTERRUPTS;
  log_buffer_add(4);
  log_buffer_add((unsigned int)format);
  log_buffer_add(parameter1);
  log_buffer_add(parameter2);
  log_buffer_add(parameter3);
  log_buffer_add(parameter4);
  ENABLE_INTERRUPTS;
}
