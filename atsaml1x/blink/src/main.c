#include "board.h"
#include <delay_systick.h>

static bool led_timer_state;

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
  led_timer_state = false;

  SysInit();

  while (1)
  {
    delayms(250);
    led_toggle();
  }
}
