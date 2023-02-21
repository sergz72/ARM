#include <stdio.h>
#include "board.h"
#include "dev_cap_meter.h"
#include "ui.h"
#include "settings.h"
#include "dev_keyboard.h"

static unsigned int last_cap_value;

void* cap_meter_initializer(void)
{
  return (void*)1;
}

void cap_meter_ui_init_handler(void* config)
{
  last_cap_value = 0xFFFFFFFF;
  cap_meter_on();
  LED_ClearScreen();
}

static unsigned int calculate_capacitance(void)
{
  return (int)(cap_value * settings[SETTING_CAP_VALUE_MUL] / CAP_VALUE_DIV);
}

void cap_meter_ui_handler(void* data, void* config)
{
  unsigned int capacitance;

  if (last_cap_value != cap_value)
  {
    last_cap_value = cap_value;
    capacitance = calculate_capacitance();
    if (capacitance < 100000)
      LED_Printf(0, 0x10, "%5d%dpF", capacitance / 10, capacitance % 10);
    else if (capacitance < 10000000)
      LED_Printf(0, 0x4, "%3d%03dnF", capacitance / 10000, (capacitance / 10) % 1000);
    else
      LED_Printf(0, 0x4, "%3d%03duF", capacitance / 10000000, (capacitance / 10000) % 1000);
  }
  cap_meter_init();
}

int cap_meter_ui_keyboard_handler(void *config, unsigned int event)
{
  if (event == KEYBOARD_EVENT_LEAVE)
    cap_meter_off();
  return 0;
}
