#include "board.h"
#include <pico/stdlib.h>
#include <stdio.h>
#include <shell.h>
#include <getstring.h>
#include "adf4351_commands.h"

static char command_line[200];

int getch_(void)
{
  int ch;
  do
  {
    ch = getchar_timeout_us(1000000);
  } while (ch == PICO_ERROR_TIMEOUT);
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
  int rc;

  SystemInit();
  stdio_init_all();

  shell_init(printf, gets_);
  register_adf4351_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  while (1)
  {
    getstring_next();
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
        rc = shell_execute(command_line);
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
