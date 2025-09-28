#include "board.h"
#include <shell.h>
#include <getstring.h>
#include <stdio.h>
#include <common_printf.h>
#include "multimeter_commands.h"
#include "multimeter.h"
#include "ui.h"

static int led_state;
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
  int cnt_led = 0;

  led_state = 0;

  shell_init(common_printf, NULL);
  register_multimeter_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  CDCInit();

  multimeter_init();

  UI_Init();

  while (1)
  {
    delayms(10);

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

    if (cnt_led == 99)
    {
      led_toggle();
      cnt_led = 0;
    }
    else
      cnt_led++;

    Process_Timer_Event();
  }
}
