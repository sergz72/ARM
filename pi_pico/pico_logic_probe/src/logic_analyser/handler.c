#include "logic_analyser.h"
#include "board.h"
#include <pico/multicore.h>
#include <memory.h>
#include "ui.h"

#define MAX_COUNTER 1000

static int counter, currentBuffer, up;
static unsigned int buffer[2][MAX_COUNTER], *pbuffer;
unsigned int pinState[32][3], v1, v2;
static struct repeating_timer la_timer;

static bool la_repeating_timer_callback(struct repeating_timer *t)
{
  int prevBuffer;
  *pbuffer++ = gpio_get_all();
  counter++;
  up = !up;
  LAInputsCommand(up ? gpio_pull_up : gpio_pull_down);
  if (counter == MAX_COUNTER)
  {
    prevBuffer = currentBuffer;
    currentBuffer = currentBuffer ? 0 : 1;
    pbuffer = buffer[currentBuffer];
    counter = 0;
    multicore_fifo_push_blocking(prevBuffer);
  }
  return true;
}

static void LAUpdateState(unsigned int pin)
{
  unsigned int mask = 1 << pin;
  if (v1 & mask)
  {
    if (v2 & mask)
      pinState[pin][1]++; // logical 1
    else
      pinState[pin][2]++; // floating
  }
  else
  {
    if (v2 & mask)
    {
      pinState[pin][0]++; // impulse
      pinState[pin][1]++;
    }
    else
      pinState[pin][0]++; // logical 0
  }
}

static void LABuildState(unsigned int bufferId)
{
  unsigned int *p = buffer[bufferId];
  memset(pinState, 0, sizeof(pinState));
  for (int i = 0; i < MAX_COUNTER; i += 2)
  {
    v1 = *p++;
    v2 = *p++;
    LAInputsCommand(LAUpdateState);
  }
}

void LACPU1Task(void)
{
  int led_state = 0;
  unsigned int bufferId;

  while (true)
  {
    bufferId = multicore_fifo_pop_blocking();
    if (bufferId > 1)
      continue;
    led_state = !led_state;
    gpio_put(PICO_DEFAULT_LED_PIN, led_state);
    LABuildState(bufferId);
    LALcdUpdate();
    SafeLcdUpdate();
  }
}

void LAStartEvents(void)
{
  counter = currentBuffer = 0;
  up = 1;
  pbuffer = buffer[currentBuffer];

  add_repeating_timer_us(100, la_repeating_timer_callback, NULL, &la_timer);
}

void LAStopEvents(void)
{
  cancel_repeating_timer(&la_timer);
}

int LAGpioCallback(unsigned int gpio, unsigned long events)
{
  return 0;
}