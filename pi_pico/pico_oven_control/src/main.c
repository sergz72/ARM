#include "board.h"
#include <pico/stdlib.h>
#include <stdio.h>
#include <shell.h>
#include <getstring.h>
#include "ui.h"

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
  int rc;
  unsigned int keyboard_status, temperature;

  SystemInit();
  stdio_init_all();

  shell_init(printf, gets_);

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  MCP9600Init();

  WS2812Init();
  controller_init();
  UI_Init();

  while (1)
  {
    delayms(500);
    if (!getstring_next()) {
      switch (command_line[0]) {
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
    keyboard_status = get_keyboard_status();
    temperature = get_temperature();
    Process_Timer_Event(keyboard_status, temperature, update_status(temperature));
  }
}
