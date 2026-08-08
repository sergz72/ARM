#include "board.h"
#include <gd32e23x_rcu.h>
#include <gd32e23x_gpio.h>

static void ports_init(void)
{
  rcu_periph_clock_enable(LED_CLOCK);
  LED_TIMER_OFF;
  gpio_mode_set(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
  gpio_output_options_set(LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_PIN);
}

void SysInit(void)
{
  ports_init();
}

void _init(void)
{
}
