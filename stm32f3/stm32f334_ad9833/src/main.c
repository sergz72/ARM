#include "board.h"
#include <serial.h>
#include <shell.h>
#include <stdio.h>
#include <stdarg.h>
#include <getstring.h>
#include <fixed_queue.h>
#include "ad9833_commands.h"
#include <ad9833.h>

static char command_line[200];
static Queue serial_q;
static unsigned char serial_queue_buffer[SERIAL_QUEUE_SIZE];

static void serial_receive(char c)
{
  queue_push(&serial_q, &c);
}

static int serial_printf(const char *format, ...)
{
  static char buffer[PRINTF_BUFFER_LENGTH], *p;
  va_list vArgs;
  int rc;

  va_start(vArgs, format);
  rc = vsnprintf(buffer, sizeof(buffer), format, vArgs);
  va_end(vArgs);
  p = buffer;
  while (*p)
  {
    if (*p == '\n')
      serial_send(SERIAL_NO, '\r');
    serial_send(SERIAL_NO, *p++);
  }
  return rc;
}

static int getch_(void)
{
  for (;;)
  {
    __disable_irq();
    unsigned char *v = (unsigned char *)queue_peek(&serial_q);
    if (v)
    {
      queue_pop(&serial_q);
      __enable_irq();
      return *v;
    }
    __enable_irq();
    delayms(10);
  }
}

static int putc_(int c)
{
  serial_send(SERIAL_NO, c);
  return 0;
}

static char *gets_with_buffer_init(const char *buffer_init)
{
  getstring(command_line, sizeof(command_line), getch_, putc_, buffer_init);
  return command_line;
}

int main(void)
{
  int rc;
  char *p;
  const char *buffer_init;

  queue_init(&serial_q, SERIAL_QUEUE_SIZE, 1, (char*)serial_queue_buffer);

  serial_init(SERIAL_NO, SERIAL_SPEED, serial_receive);
  shell_init(serial_printf, NULL);
  register_ad9833_commands();
  ad9833_set_config(0, 0);

  buffer_init = NULL;
  while (1)
  {
    serial_printf("\r\33[2K$ ");
    p = gets_with_buffer_init(buffer_init);
    switch (p[0])
    {
      case SHELL_UP_KEY:
        buffer_init = shell_get_prev_from_history();
        continue;
      case SHELL_DOWN_KEY:
        buffer_init = shell_get_next_from_history();
        continue;
      default:
        buffer_init = NULL;
        rc = shell_execute(p);
        if (rc == 0)
          serial_puts(SERIAL_NO, "OK\r\n");
        else if (rc < 0)
          serial_puts(SERIAL_NO, "Invalid command line\r\n");
        else
          serial_printf("shell_execute returned %d\n", rc);
        break;
    }
  }
}
