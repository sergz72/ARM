#include "board.h"
#include "ui.h"
#include <pico/multicore.h>
#include <memory.h>
#include "keyboard.h"

#define MAX_COUNTER 1000

static int counter, currentBuffer, up;
static unsigned int buffer[2][MAX_COUNTER], *pbuffer;
unsigned int pinState[32][3], v1, v2;

static bool repeating_timer_callback(struct repeating_timer *t)
{
  int prevBuffer;
  *pbuffer++ = gpio_get_all();
  counter++;
  up = !up;
  InputsCommand(up ? gpio_pull_up : gpio_pull_down);
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

static void UpdateState(unsigned int pin)
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
    if (!(v2 & mask))
      pinState[pin][0]++; // logical 0
  }
}

static void BuildState(unsigned int bufferId)
{
  unsigned int *p = buffer[bufferId];
  memset(pinState, 0, sizeof(pinState));
  for (int i = 0; i < MAX_COUNTER; i += 2)
  {
    v1 = *p++;
    v2 = *p++;
    InputsCommand(UpdateState);
  }
}

void CPU1Task(void)
{
  int led_state = 0;
  unsigned int bufferId;

  while (true)
  {
    bufferId = multicore_fifo_pop_blocking();
    led_state = !led_state;
    gpio_put(PICO_DEFAULT_LED_PIN, led_state);
    BuildState(bufferId);
    Process_Timer_Event();
  }
}

int main()
{
  SystemInit();

  UI_Init();
  KeyboardInit();

  counter = currentBuffer = 0;
  up = 1;
  pbuffer = buffer[currentBuffer];

  multicore_launch_core1(CPU1Task);

  struct repeating_timer timer;
  add_repeating_timer_us(100, repeating_timer_callback, NULL, &timer);

  while (true)
  {
    asm volatile ("wfi");
  }
}
