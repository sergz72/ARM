#include "board.h"
#include "ui.h"
#include "devices.h"
#include "dev_keyboard.h"
#include <getstring.h>
#include <pico/stdio.h>
#include <stdio.h>
#include <pico/time.h>

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

int main(void)
{
  int cnt, data_ready;
  unsigned int keyboard_status;
  int rc;

  SystemInit();
  stdio_init_all();

  shell_init(printf, gets_);

  sleep_ms(1000); // time for peripheral devices to initialize

  BuildDeviceList();

  BuildShellCommands();

  UI_Init();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  cnt = 0;
  while(1)
  {
    sleep_ms(100);
    if (!getstring_next())
    {
      switch (command_line[0])
      {
        case SHELL_UP_KEY:
          puts("\r\33[2K$ ");
          getstring_buffer_init(shell_get_prev_from_history());
          break;
        case SHELL_DOWN_KEY:
          puts("\r\33[2K$ ");
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
    BuildDeviceData(cnt++);
    keyboard_status = keyboard_get_filtered_status();
    data_ready = cnt == 10;
    if (data_ready)
      cnt = 0;
    Process_Timer_Event(data_ready, keyboard_status);
  }
}
