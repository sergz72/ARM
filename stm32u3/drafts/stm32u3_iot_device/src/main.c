#include "board.h"
#include <shell.h>
#include <getstring.h>
#include <stdio.h>
#include <common_printf.h>
#include <cc1101_commands.h>
#include "scd41_commands.h"
#include "date_commands.h"

static bool led_state;
static char command_line[200];

static void led_toggle(void)
{
  led_state = !led_state;
  if (led_state)
    LED_ON;
  else
    LED_OFF;
}

int main(void)
{
  int rc;
  int led_counter = 0;

  led_state = false;

  shell_init(common_printf, nullptr);
  register_cc1101_commands();
  register_scd41_commands();
  register_date_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  usart_start();

  while (1)
  {
    delayms(100);
    if (led_counter == 9)
    {
      led_counter = 0;
      led_toggle();
    }
    else
      led_counter++;
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
          puts_("OK\r\n$ ");
        else if (rc < 0)
          puts_("Invalid command line\r\n$ ");
        else
          common_printf("shell_execute returned %d\n$ ", rc);
        break;
      }
    }
  }
}
