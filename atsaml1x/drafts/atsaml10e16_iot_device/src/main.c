#include "board.h"
#include <cc1101_commands.h>
#include "scd41_commands.h"
#include "ds1307_commands.h"
#include "ds3231_commands.h"
#include "pcf8563_commands.h"
#include "security_commands.h"
#include "eeprom_commands.h"
#include "sht_commands.h"
#include "adc_commands.h"
#include "config.h"
#include <main_loop.h>

void configuration_update_callback(void)
{
  if (configuration_loaded)
    LED_BATTERY_OFF;
  else
    LED_BATTERY_ON;
}

void register_commands(void)
{
  register_cc1101_commands();
  register_scd41_commands();
  register_ds1307_commands();
  register_ds3231_commands();
  register_pcf8563_commands();
  register_security_commands();
  register_eeprom_commands();
  register_sht_commands();
  register_adc_commands();
}

int main(void)
{
  SysInit();

  load_configuration();

  main_loop();
}
