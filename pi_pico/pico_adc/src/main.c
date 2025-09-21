#include <stdio.h>
#include <shell.h>
#include <pico/stdio.h>
#include <pico/time.h>
#include "getstring.h"
#include "board.h"
#include "ads1259_commands.h"
#include "ads1232_commands.h"
#include "ad7124_commands.h"
#include "ad7705_commands.h"
#include "ad7793_commands.h"
#include <ads1259.h>
#include <ad7124.h>
#include <ad7793.h>
#include <ad7705.h>
#include <ad7793.h>

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
  int counter = 0;

  SystemInit();
  stdio_init_all();

  shell_init(printf, gets_);
  //register_ads1259_commands();
  //register_ads1232_commands();
  //register_ad7124_commands();
  //register_ad7705_commands();
  register_ad7793_commands();

  WS2812Init();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  ads1259_reset();
  ad7124_init();
  ad7705_init();
  ad7793_init();

  while (1)
  {
    sleep_ms(100);

    if (counter == 9)
    {
      put_pixel(urgb_u32(0, led_state ? 0x10 : 0, 0)); // green
      led_state = !led_state;
      counter = 0;
    }
    else
      counter++;

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
