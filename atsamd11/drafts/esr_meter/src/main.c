#include "board.h"

volatile int interrupt_request;
unsigned int value;
static int led_timer_state;

const unsigned char config = 1;

const void *txbufs[2] = { &value, &config };

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
  led_timer_state = 0;
  interrupt_request = 0;
  value = 12345;

  SysInit();

  while (1)
  {
    delayms(250);
    led_toggle();
    if (!interrupt_request)
    {
      if (BUTTON_PRESSED)
      {
      }
      INTERRUPT_FLAG_SET;
      interrupt_request = 1;
    }
  }
}
