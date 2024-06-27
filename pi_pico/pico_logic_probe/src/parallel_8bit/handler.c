#include "board.h"
#include "parallel_8bit.h"
#include <pico/multicore.h>
#include "ui.h"

volatile unsigned int p8code;

void P8CPU1Task(void)
{
  int led_state = 0;

  while (true)
  {
    p8code = multicore_fifo_pop_blocking();
    led_state = !led_state;
    gpio_put(PICO_DEFAULT_LED_PIN, led_state);
    P8LcdUpdate();
    SafeLcdUpdate();
  }
}

int P8GpioCallback(unsigned int gpio, unsigned long events)
{
  if (gpio == P8_WR)
  {
    unsigned int code = gpio_get_all();
    multicore_fifo_push_blocking(code);
    return 1;
  }
  return 0;
}

void P8StartEvents(void)
{
  gpio_set_irq_enabled(P8_WR, GPIO_IRQ_EDGE_RISE, true);
}

void P8StopEvents(void)
{
  gpio_set_irq_enabled(P8_WR, GPIO_IRQ_EDGE_RISE, false);
}
