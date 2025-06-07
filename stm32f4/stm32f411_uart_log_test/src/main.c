#include "board.h"
#include "delay_systick.h"
#include <uart_log.h>

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

  uart_log_init();
  uart_log_printf1("test %d", 1);

  while (1)
  {
    delayms(1000);
    led_toggle();
    uart_log_process();
  }
}
