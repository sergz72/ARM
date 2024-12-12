#include <stdio.h>
#include <shell.h>
#include <spi_commands.h>
#include <pico/stdio.h>
#include <pico/time.h>
#include "getstring.h"
#include "spi_memory_commands.h"
#include "board.h"

#define MAX_COMMAND_LINE_LENGTH 128

char command_line[MAX_COMMAND_LINE_LENGTH];

int getch_(void)
{
  int ch = getchar_timeout_us(0);
  if (ch == PICO_ERROR_TIMEOUT)
    return EOF;
  return ch;
}

char *gets_(void)
{
  getstring_next();
  return command_line;
}

void puts_(const char *s)
{
  while (*s)
    putchar(*s++);
}

int main(void)
{
  int rc;
  bool led_state = false;

  SystemInit();
  stdio_init_all();

  shell_init(printf, gets_);
  register_spi_commands();
  register_spi_memory_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  while (1)
  {
    sleep_ms(100);

    gpio_put(PICO_DEFAULT_LED_PIN, led_state);
    led_state = !led_state;

    if (!getstring_next())
    {
      switch (command_line[0])
      {
        case SHELL_UP_KEY:
          puts_("\r\33[2K$ ");
          getstring_buffer_init(shell_get_prev_from_history());
          break;
        case SHELL_DOWN_KEY:
          puts_("\r\33[2K$ ");
          getstring_buffer_init(shell_get_next_from_history());
          break;
        default:
          rc = shell_execute(command_line);
          if (rc == 0)
            printf("OK\n$ ");
          else if (rc < 0)
            printf("Invalid command line\n$ ");
          else
            printf("shell_execute returned %d\n$ ", rc);
          break;
      }
    }
  }
}