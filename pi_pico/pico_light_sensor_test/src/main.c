#include "board.h"
#include <pico/stdlib.h>
#include <stdio.h>
#include <shell.h>
#include <getstring.h>
#include "i2c_commands.h"
#include "bh_commands.h"
#include "tsl_commands.h"
#include "veml_commands.h"
#include "ws2812.h"

static char command_line[200];

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

int main()
{
  int counter = 0;

  SystemInit();
  stdio_init_all();

  WS2812Init();

  shell_init(printf, gets_);
  register_i2c_commands();
  register_bh_commands();
  register_tsl_commands();
  register_veml_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  while (1)
  {
    sleep_ms(100);
    counter++;

    if (counter == 10)
      put_pixel(urgb_u32(0, 0x10, 0)); // gree
    if (counter == 20)
    {
      counter = 0;
      put_pixel(urgb_u32(0, 0, 0)); // gree
    }

    if (!getstring_next())
    {
      switch (command_line[0])
      {
      case SHELL_UP_KEY:
        puts_("\r\33[2K$ ");
        getstring_buffer_init(shell_get_prev_from_history());
        continue;
      case SHELL_DOWN_KEY:
        puts_("\r\33[2K$ ");
        getstring_buffer_init(shell_get_next_from_history());
        continue;
      default:
        int rc = shell_execute(command_line);
        if (rc == 0)
          puts_("OK\r\n$ ");
        else if (rc < 0)
          puts_("Invalid command line\r\n$ ");
        else
          printf("shell_execute returned %d\n$ ", rc);
        break;
      }
    }
  }
}
