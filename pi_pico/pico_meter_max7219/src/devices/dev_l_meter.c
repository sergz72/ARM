#include "dev_l_meter.h"
#include <dev_freq_pwm.h>
#include "ui.h"
#include "dev_keyboard.h"
#include "settings.h"

static unsigned int last_counter_value;

void* l_meter_initializer(void)
{
  return (void*)1;
}

void l_meter_ui_init_handler(void* config)
{
  last_counter_value = 0xFFFFFFFF;
  freq_pwm_on(1, 2);
  LED_ClearScreen();
}

/*
 * L = (NOM / F / F / C) * 100 / PI2
 * C in pF
 * L in uH
 * F in HZ
 */
#define NOM (2500000000 * 100000000)
#define PI2 987
unsigned int calculate_inductance(void)
{
  unsigned long long int l;
  unsigned long long int f2;
  unsigned long long int c = settings[SETTING_L_CAP] / 100;
  if (!c)
    return 0;
  f2 = (unsigned long long int)last_counter_value * last_counter_value;
  l = NOM / f2 / c;
  return l * 100 / PI2;
}

void l_meter_ui_handler(void* data, void* config)
{
  unsigned int inductance;
  unsigned int v = get_counter_value2();
  if (v != last_counter_value)
  {
    last_counter_value = v;
    inductance = calculate_inductance();
    if (inductance < 1000000)
      LED_Printf(0, 0x8, "%4d%02duH", inductance / 100, inductance % 100);
    else
      LED_Printf(0, 0x8, "%4d%02dmH", inductance / 100000, (inductance / 1000) % 100);
  }
}

int l_meter_ui_keyboard_handler(void *config, unsigned int event)
{
  if (event == KEYBOARD_EVENT_LEAVE)
    freq_pwm_off();
  return 0;
}
