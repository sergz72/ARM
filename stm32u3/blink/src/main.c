#include "board.h"
#include <delay_systick.h>

int led_state;

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
  led_state = 0;

  while (1)
  {
    delayms(1000);
    led_toggle();
  }
}
