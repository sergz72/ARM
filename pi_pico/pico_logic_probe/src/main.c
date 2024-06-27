#include "board.h"
#include "ui.h"
#include "keyboard.h"
#include "logic_analyser/logic_analyser.h"
#include "parallel_8bit/parallel_8bit.h"
#include <pico/multicore.h>

#define MODE_LOGIC_ANALYSER 0
#define MODE_PARALLEL_8BIT  1
#define MAX_MODE            1

static int mode;

typedef struct {
  void (*start_events)(void);
  void (*cpu1_task)(void);
  void (*stop_events)(void);
  void (*lcd_init)(void);
  void (*init)(void);
  int (*gpio_callback)(unsigned int gpio, unsigned long events);
} Mode;

const Mode modes[MAX_MODE + 1] = {
    {
        .start_events = LAStartEvents,
        .stop_events = LAStopEvents,
        .lcd_init = LALcdInit,
        .cpu1_task = LACPU1Task,
        .init = LAInit,
        .gpio_callback = LAGpioCallback
    },
    {
        .start_events = P8StartEvents,
        .stop_events = P8StopEvents,
        .lcd_init = P8LcdInit,
        .cpu1_task = P8CPU1Task,
        .init = P8Init,
        .gpio_callback = P8GpioCallback
    }
};

static void SwitchMode()
{
  modes[mode].stop_events();
  requestToStop = 1;
  do
  {
    sleep_us(10);
  } while (doNotResetCPU1);
  multicore_reset_core1();
  multicore_fifo_drain();
  requestToStop = 0;
  mode++;
  if (mode > MAX_MODE)
    mode = 0;
  modes[mode].init();
  LcdScreenFill(BLACK_COLOR); // clear screen
  modes[mode].lcd_init();
  multicore_launch_core1(modes[mode].cpu1_task);
  modes[mode].start_events();
}

void gpio_callback(unsigned int gpio, unsigned long events)
{

  if (!modes[mode].gpio_callback(gpio, events))
    keyboard_gpio_callback(gpio, events);
}

int main()
{
  SystemInit();

  gpio_set_irq_callback(gpio_callback);
  irq_set_enabled(IO_IRQ_BANK0, true);

  UI_Init();
  KeyboardInit();

  mode = MODE_LOGIC_ANALYSER;

  LALcdInit();
  multicore_launch_core1(LACPU1Task);
  LAStartEvents();

  while (true)
  {
    switch (buttonEvent)
    {
      case IDBUTTONB:
        break;
      case IDBUTTONA:
        SwitchMode();
        break;
      case IDBUTTONCENTER:
        break;
      case IDBUTTONTOP:
        break;
      case IDBUTTONBOTTOM:
        break;
      case IDBUTTONLEFT:
        break;
      case IDBUTTONRIGHT:
        break;
    }
    buttonEvent = IDNOEVENT;
    asm volatile ("wfi");
  }
}
