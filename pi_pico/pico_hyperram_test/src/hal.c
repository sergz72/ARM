#include "board.h"
#include <hardware/gpio.h>
#include "hyperram.h"

static void LEDInit(void)
{
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void SystemInit(void)
{
  LEDInit();
  hyperram_init(6);
}
