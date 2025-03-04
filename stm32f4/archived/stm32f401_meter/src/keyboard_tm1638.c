#include "board.h"
#include "keyboard_tm1638.h"
#include <tm1638.h>
#include <seven_seg.h>

int keyboard_init(void)
{
  if (keyboard_present)
  {
    tm1638_init();
    return 0;
  }
  return 1;
}

void keyboard_set_leds(unsigned int led_state)
{
  if (keyboard_present)
    tm1638_set_leds(led_state);
}

void keyboard_set_display(unsigned char *symbols)
{
  int i;
  unsigned char converted[8];

  if (keyboard_present)
  {
    for (i = 0; i < 8; i++)
      converted[i] = seven_seg_get_symbol(symbols[i]);
    tm1638_out(0, 8, converted);
  }
}

unsigned int keyboard_get_status(void)
{
  if (keyboard_present)
    return tm1638_key_state();
  return 0;
}
