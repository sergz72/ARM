#include "board.h"
#include <pico/stdlib.h>
#include <stdio.h>
#include <shell.h>
#include <getstring.h>
#include "i2c_commands.h"
#include "tm1638_commands.h"
#include <tm1638.h>

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
  getstring(command_line, sizeof(command_line), getch_, putchar, NULL);
  return command_line;
}

char *gets_with_buffer_init(const char *buffer_init)
{
  getstring(command_line, sizeof(command_line), getch_, putchar, buffer_init);
  return command_line;
}

int main()
{
  int rc;
  char *p;
  const char *buffer_init;

  SystemInit();
  stdio_init_all();

  tm1638_init();

  shell_init(printf, gets_);
  register_i2c_commands();
  register_tm1638_commands();

  buffer_init = NULL;
  while (1)
  {
    printf("\r\33[2K$ ");
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
          puts("OK");
        else if (rc < 0)
          puts("Invalid command line");
        else
          printf("shell_execute returned %d\n", rc);
        break;
    }
  }
}
