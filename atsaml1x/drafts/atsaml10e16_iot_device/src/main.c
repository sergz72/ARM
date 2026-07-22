#include "board.h"
#include <shell.h>
#include <getstring.h>
#include <cc1101_commands.h>
#include "scd41_commands.h"
#include "ds1307_commands.h"
#include "ds3231_commands.h"
#include "pcf8563_commands.h"
#include "security_commands.h"
#include "eeprom_commands.h"
#include "sht_commands.h"
#include "adc_commands.h"
#include <common_printf.h>
#include <usart.h>
#include <pwr.h>
#include "config.h"
#include "env_func.h"
#include "time_func.h"
#include "rf_modem_func.h"

static char command_line[200];
static unsigned int next_communication_time;

void configuration_update_callback(void)
{
  if (configuration_loaded)
    LED_BATTERY_OFF;
  else
    LED_BATTERY_ON;
}

int main(void)
{
  int rc;

  SysInit();

  load_configuration();

  shell_init(common_printf, nullptr);
  register_cc1101_commands();
  register_scd41_commands();
  register_ds1307_commands();
  register_ds3231_commands();
  register_pcf8563_commands();
  register_security_commands();
  register_eeprom_commands();
  register_sht_commands();
  register_adc_commands();

  getstring_init(command_line, sizeof(command_line), getch_, puts_);

  next_communication_time = timestamp;

  while (1)
  {
    enter_standby();
    if (timer_interrupt)
    {
      timer_interrupt = false;
      if (configuration_loaded && timestamp >= next_communication_time)
      {
        next_communication_time = timestamp + SEND_INTERVAL;
        if (!sensor_get())
        {
          encrypt_env();
          send_env();
        }
      }
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
