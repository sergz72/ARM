#include "board.h"
#include "ui.h"
#include <tm1638.h>
#include <seven_seg.h>
#include <stdio.h>
#include <cap_meter.h>
#include <hardware/pio.h>

#define MODE_FREQUENCY   1
#define MODE_CAPACITANCE 2

static unsigned int last_counter_value, mode, last_cap_value;

void UI_Init(void)
{
  tm1638_init();

  last_counter_value = last_cap_value = 0xFFFFFFFF;
  mode = MODE_FREQUENCY;
}

void LED_Write_String(char *string)
{
  unsigned char buffer[8];
  int i;

  for (i = 0; i < 8; i++)
    buffer[i] = seven_seg_get_symbol(*string++);
  tm1638_out(0, 8, buffer);
}

void Process_Timer_Event(void)
{
  char buffer[9];
  unsigned int capacitance;
  unsigned int m = tm1638_key_state();

  if (m && m != mode)
  {
    mode = m;
    last_counter_value = last_cap_value = 0xFFFFFFFF;
    switch (mode)
    {
      case MODE_FREQUENCY:
        pio_sm_set_enabled(pio0, 0, true);
        pio_sm_set_enabled(pio0, 1, true);
        pio_sm_set_enabled(pio0, 2, false);
        break;
      case MODE_CAPACITANCE:
        pio_sm_set_enabled(pio0, 0, false);
        pio_sm_set_enabled(pio0, 1, false);
        pio_sm_clear_fifos(pio0, 2);
        gpio_set_dir(CAP_RLOW_PIN, false);
        pio_sm_put_blocking(pio0, 2, CAP_MAX);
        pio_sm_set_enabled(pio0, 2, true);
        break;
    }
  }

  switch (mode)
  {
    case MODE_FREQUENCY:
      if (counter_value != last_counter_value)
      {
        last_counter_value = counter_value;
        if (counter_value < 100000000)
        {
          sprintf(buffer, "%8d", (unsigned int)((unsigned long long int)counter_value * KOEF_MUL / KOEF_DIV));
          LED_Write_String(buffer);
        }
        else
          LED_Write_String("HI      ");
      }
      break;
    case MODE_CAPACITANCE:
      if (last_cap_value != cap_value)
      {
        last_cap_value = cap_value;
        capacitance = calculate_capacitance();
        if (capacitance < 10000)
          sprintf(buffer, "%4d.%dpF", capacitance / 10, capacitance % 10);
        else if (capacitance < 10000000)
          sprintf(buffer, "%3d.%02dnF", capacitance / 10000, (capacitance / 100) % 100);
        else
          sprintf(buffer, "%3d.%02duF", capacitance / 10000000, (capacitance / 100000) % 100);
        LED_Write_String(buffer);
      }
      cap_meter_init();
      break;
    default:
      LED_Write_String("ERROR   ");
      break;
  }
}
