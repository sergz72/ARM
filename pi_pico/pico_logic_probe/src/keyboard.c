#include "keyboard.h"
#include "ui.h"
#include "board.h"
#include <pico/time.h>

typedef struct {
  unsigned int pin;
  int event;
} PinMapping;

volatile int buttonEvent;
uint64_t disabledUntil;

static const PinMapping pinMapping[] = {
    {KEY_UP, IDBUTTONRIGHT}, {KEY_DOWN, IDBUTTONLEFT}, {KEY_CENTER, IDBUTTONCENTER},
    {KEY_LEFT, IDBUTTONTOP}, {KEY_RIGHT, IDBUTTONBOTTOM}, {KEY_A, IDBUTTONA}, {KEY_B, IDBUTTONB}
};

static void gpio_callback(unsigned int gpio, unsigned long events)
{
  int i;
  PinMapping m;
  uint64_t time = to_us_since_boot(get_absolute_time());

  if (time < disabledUntil)
    return;

  disabledUntil = time + 50000; // 50 ms

  if (events & GPIO_IRQ_EDGE_FALL)
  {
    for (i = 0; i < MAX_KEY; i++)
    {
      m = pinMapping[i];
      if (gpio == m.pin)
      {
        buttonEvent = m.event;
        break;
      }
    }
  }
}

static void KeyConfig(unsigned int Pin)
{
  gpio_init(Pin);
  gpio_pull_up(Pin);
  gpio_set_dir(Pin, GPIO_IN);
  gpio_set_irq_enabled_with_callback(Pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

void KeyboardInit(void)
{
  int i;

  buttonEvent = IDNOEVENT;
  disabledUntil = 0;

  for (i = 0; i < MAX_KEY; i++)
    KeyConfig(pinMapping[i].pin);
}
