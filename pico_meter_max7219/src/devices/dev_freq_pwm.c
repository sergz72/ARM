#include "board.h"
#include <dev_freq_pwm.h>
#include "ui.h"
#include "dev_keyboard.h"
#include "settings.h"

/*

Default menu: C1F DUEn

C1 - channel number
F  - frequency modify
DU - duty modify
En - enable/disable

Default channel data:
FFFFHDDD

FFFF - frequency
H - kHz symbol (oe space - Hz symbol)
DDD - duty

*/

#define MODE_NORMAL 0
#define MODE_ENTER  1

typedef struct {
  unsigned int frequency;
  unsigned int duty;
  int enabled;
} PWM_Channel;

static unsigned int last_counter_value1, last_counter_value2;
static int current_channel, default_menu, mode;
static PWM_Channel channel[2];

static void ChannelInit(int c)
{
  channel[c].duty = DEFAULT_PWM_DUTY;
  channel[c].frequency = DEFAULT_PWM_FREQ;
  channel[c].enabled = 0;
}

void *freq_pwm_initializer(void)
{
  current_channel = default_menu = mode = 0;
  ChannelInit(0);
  ChannelInit(1);
  return (void*)1;
}

static void ShowMenu(void)
{
  if (mode == MODE_NORMAL)
    LED_Printf(3, 0x0C << (default_menu << 1), "C%dF DU%s", current_channel + 1, channel[current_channel].enabled ? "DI" : "En");
  else
    LED_Write_String(3, 0, "KHHZ    ");
}

static void ShowChannel(void)
{
  char kHz = ' ';
  unsigned int f = channel[current_channel].frequency;
  if (f >= 10000)
  {
    f /= 1000;
    kHz = 'K';
  }
  LED_Printf(2, 0x44, "%04d%c%03d", f, kHz, channel[current_channel].duty);
}

void freq_pwm_ui_init_handler(void* config)
{
  last_counter_value1 = last_counter_value2 = 0xFFFFFFFF;
  freq_pwm_on();
  LED_ClearScreen();
  ShowChannel();
  ShowMenu();
}

void ShowCounter(int line, unsigned int value)
{
  value = (unsigned int)((unsigned long long int)value * settings[SETTING_MCLK] / DEFAULT_MCLK);
  if (value < 100000000)
    LED_Printf(line, 0, "%8d", value);
 else
    LED_Write_String(line, 0, "HI      ");
}

void freq_pwm_ui_handler(void* data, void* config)
{
  if (counter_value1 != last_counter_value1)
  {
    last_counter_value1 = counter_value1;
    ShowCounter(0, last_counter_value1);
  }
  if (counter_value2 != last_counter_value2)
  {
    last_counter_value2 = counter_value2;
    ShowCounter(1, last_counter_value2);
  }
}

static void ToggleChannelOutput(void)
{
  int enabled = channel[current_channel].enabled ? 0 : 1;
  channel[current_channel].enabled = enabled;
  if (enabled)
    pwm_on(current_channel);
  else
    pwm_off(current_channel);
}

int freq_pwm_ui_keyboard_handler(void *config, unsigned int event)
{
  switch (event)
  {
  case KEYBOARD_EVENT_ENTER:
    if (mode == MODE_ENTER)
    {
      mode = MODE_NORMAL;
      return 1;
    }
    break;
  case KEYBOARD_EVENT_LEAVE:
    if (mode == MODE_ENTER)
    {
      mode = MODE_NORMAL;
      ShowChannel();
      ShowMenu();
      return 1;
    }
    channel[0].enabled = channel[1].enabled = 0;
    freq_pwm_off();
    break;
  case KEYBOARD_EVENT_F1:
    current_channel = current_channel == 0 ? 1 : 0;
    ShowChannel();
    ShowMenu();
    return 1;
  case KEYBOARD_EVENT_F2:
    if (default_menu != 0)
    {
      default_menu = 0;
      ShowMenu();
      return 1;
    }
    break;
  case KEYBOARD_EVENT_F3:
    if (default_menu != 1)
    {
      default_menu = 1;
      ShowMenu();
      return 1;
    }
    break;
  case KEYBOARD_EVENT_F4:
    ToggleChannelOutput();
    ShowMenu();
    return 1;
  default:
    if (mode == MODE_NORMAL)
    {
      mode = MODE_ENTER;
      if (default_menu)
        enableCursor(2, 5, 7);
      else
        enableCursor(2, 0, 3);
    }
    return 1;
  }
  return 0;
}
