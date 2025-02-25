#include "board.h"
#include <dl_gpio.h>

static void toggle_led(void)
{
  DL_GPIO_togglePins(LED_GPIO, LED_PIN);
}

int main(void)
{
  SystemInit();

  while (1)
  {
    delayms(500);
    toggle_led();
  }
}
