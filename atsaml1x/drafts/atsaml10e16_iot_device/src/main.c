#include "board.h"
#include <shell.h>
#include <getstring.h>
#include <cc1101_commands.h>
#include "scd41_commands.h"
//#include "ds1307_commands.h"
#include "ds3231_commands.h"
//#include "pcf8563_commands.h"
#include "security_commands.h"
#include "eeprom_commands.h"
#include "sht_commands.h"
#include <common_printf.h>
#include <usart.h>
#include <pwr.h>
#include <rtc_ds3231.h>

#include "security_commands.h"

static bool led_timer_state;
static char command_line[200];

static void led_toggle(void)
{
  led_timer_state = !led_timer_state;
  if (led_timer_state)
    LED_TIMER_ON;
  else
    LED_TIMER_OFF;
}

int main(void)
{
  int rc;

  led_timer_state = false;

  SysInit();

  if (ds3231_init(1))
  {
    LED_TIMER_ON;
    while (1)
      __WFI();
  }

  shell_init(common_printf, nullptr);
  register_cc1101_commands();
  register_scd41_commands();
  //register_ds1307_commands();
  register_ds3231_commands();
  //register_pcf8563_commands();
  register_security_commands();
  register_eeprom_commands();
  register_sht_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  while (1)
  {
    enter_standby();
    if (timer_interrupt)
    {
      timer_interrupt = false;
      led_toggle();
    }
    if (chars_received())
    {
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
}
