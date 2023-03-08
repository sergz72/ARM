#include "board.h"
#include <dev_freq_pwm.h>
#include "ui.h"
#include "dev_keyboard.h"
#include "settings.h"
#include <stdlib.h>
//#include <stdio.h>

/*

Default menu: C1F1D1En

C1 - channel number (1/2)
F1  - frequency modify
D1 - duty modify
En - enable/disable

Default channel data:
FFFFHDDD

FFFF - frequency
H - kHz symbol (space - Hz symbol)
DDD - duty

Frequency counter menu: F1  1 10

F1 - frequency counter channel (1/2)
1  - one second measurement. Resolution is 1 Hz
10 - ten second measurement. Resolution is .1 Hz

*/

typedef struct {
  unsigned int frequency;
  unsigned int duty;
  int enabled;
} PWM_Channel;

static unsigned int last_counter_value1, last_counter_value2, temp_value, measurement_interval,
                    frequency_multiplier, duty_multiplier, frequency_range;
static int current_channel, default_menu, frequency_counter_menu;
static PWM_Channel channel[2];

static void ChannelInit(int c)
{
  channel[c].duty = DEFAULT_PWM_DUTY;
  channel[c].frequency = DEFAULT_PWM_FREQ;
  channel[c].enabled = 0;
}

void *freq_pwm_initializer(void)
{
  current_channel = default_menu = frequency_counter_menu = 0;
  measurement_interval = frequency_range = 1;
  frequency_multiplier = duty_multiplier = 0;
  ChannelInit(0);
  ChannelInit(1);
  return (void*)1;
}

static void ShowMenu(void)
{
  if (frequency_counter_menu)
    LED_Write_String(3, 0, "F   1 10");
  else
  {
    if (!cursorEnabled)
      LED_Printf(3, 0x0C << (default_menu << 1), "C%dF%dD%d%s",
                 current_channel + 1,
                 frequency_multiplier + 1,
                 duty_multiplier + 1,
                 channel[current_channel].enabled ? "DI" : "En");
    else
      LED_Write_String(3, 0, "1 2 3 4 ");
  }
}

static void ShowChannel(void)
{
  char kHz = 'K';
  unsigned int dots = 0x40;
  unsigned int f = channel[current_channel].frequency;
  unsigned int d = channel[current_channel].duty;
  if (cursorEnabled)
  {
    if (default_menu)
      d = temp_value;
    else
      f = temp_value;
  }
  f /= frequency_range;
  switch (frequency_range)
  {
  case 1:
    kHz = ' ';
    break;
  case 10:
    dots |= 2;
    break;
  case 100:
    dots |= 4;
    break;
  }
  LED_Printf(2, dots, "%04d%c%03d", f, kHz, d);
}

void freq_pwm_ui_init_handler(void* config)
{
  last_counter_value1 = last_counter_value2 = 0xFFFFFFFF;
  freq_pwm_on(measurement_interval, 3);
  LED_ClearScreen();
  ShowChannel();
  ShowMenu();
}

void ShowCounter(int line, unsigned int value)
{
  value = (unsigned int)((unsigned long long int)value * settings[SETTING_MCLK] / DEFAULT_MCLK);
  if (value < 100000000)
    LED_Printf(line, measurement_interval == 10 ? 0x40 : 0, "%8d", value);
  else
    LED_Write_String(line, 0, "HI      ");
}

void freq_pwm_ui_handler(void* data, void* config)
{
  unsigned int v = get_counter_value1();
  if (v != last_counter_value1)
  {
    last_counter_value1 = v;
    ShowCounter(0, last_counter_value1);
  }
  v = get_counter_value2();
  if (v != last_counter_value2)
  {
    last_counter_value2 = v;
    ShowCounter(1, last_counter_value2);
  }
}

static void ToggleChannelOutput(void)
{
  int enabled = channel[current_channel].enabled ? 0 : 1;
  channel[current_channel].enabled = enabled;
  if (enabled)
  {
    pwm_set_freq(current_channel, channel[current_channel].frequency, channel[current_channel].duty);
    pwm_on(current_channel);
  }
  else
    pwm_off(current_channel);
}

static void UpdateFrequency(unsigned int coef)
{
  if (!temp_value)
    temp_value = coef;
  else
    temp_value *= coef;
  if (temp_value < MINIMUM_PWM_FREQ)
    temp_value = MINIMUM_PWM_FREQ;
  channel[current_channel].frequency = temp_value;
  if (channel[current_channel].enabled)
    pwm_set_freq(current_channel, temp_value, channel[current_channel].duty);
  cursorEnabled = 0;
  frequency_range = coef;
  ShowChannel();
  ShowMenu();
}

int freq_pwm_ui_keyboard_handler(void *config, unsigned int event)
{
  switch (event)
  {
  case KEYBOARD_EVENT_ENTER:
    if (cursorEnabled)
    {
      if (default_menu)
      {
        channel[current_channel].duty = temp_value;
        if (channel[current_channel].enabled)
          pwm_set_freq(current_channel, channel[current_channel].frequency, temp_value);
        cursorEnabled = 0;
        ShowChannel();
        return 1;
      }
    }
    else
      display_toggle();
    break;
  case KEYBOARD_EVENT_LEAVE:
    if (cursorEnabled)
    {
      cursorEnabled = 0;
      ShowChannel();
      ShowMenu();
      return 1;
    }
    channel[0].enabled = channel[1].enabled = 0;
    freq_pwm_off();
    break;
  case KEYBOARD_EVENT_F1:
    if (cursorEnabled)
    {
      if (!default_menu)
      {
        UpdateFrequency(1);
        return 1;
      }
      break;
    }
    if (frequency_counter_menu)
      frequency_counter_menu = 0;
    else
      current_channel++;
    if (current_channel == 2)
    {
      current_channel = 0;
      frequency_counter_menu = 1;
    }
    if (!frequency_counter_menu)
      ShowChannel();
    ShowMenu();
    return 1;
  case KEYBOARD_EVENT_F2:
    if (cursorEnabled)
    {
      if (!default_menu)
      {
        UpdateFrequency(10);
        return 1;
      }
      break;
    }
    if (frequency_counter_menu)
      break;
    if (default_menu != 0)
      default_menu = 0;
    else
      frequency_multiplier = (frequency_multiplier + 1) & 3;
    ShowMenu();
    return 1;
  case KEYBOARD_EVENT_F3:
    if (cursorEnabled)
    {
      if (!default_menu)
      {
        UpdateFrequency(100);
        return 1;
      }
      break;
    }
    if (frequency_counter_menu)
    {
      measurement_interval = 1;
      freq_counter_set_interval(measurement_interval);
      frequency_counter_menu = 0;
      ShowChannel();
      ShowMenu();
    }
    else
    {
      if (default_menu != 1)
        default_menu = 1;
      else
        duty_multiplier = (duty_multiplier + 1) % 3;
      ShowMenu();
    }
    return 1;
  case KEYBOARD_EVENT_F4:
    if (cursorEnabled)
    {
      if (!default_menu)
      {
        UpdateFrequency(1000);
        return 1;
      }
      break;
    }
    if (frequency_counter_menu)
    {
      measurement_interval = 10;
      freq_counter_set_interval(measurement_interval);
      frequency_counter_menu = 0;
      ShowChannel();
    }
    else
      ToggleChannelOutput();
    ShowMenu();
    return 1;
  default:
    if (!cursorEnabled)
    {
      if (default_menu)
        enableCursor(2, 5, 7);
      else
      {
        enableCursor(2, 0, 3);
        ShowMenu();
      }
      temp_value = event - 1;
    }
    else
    {
      temp_value = temp_value * 10 + event - 1;
      if (default_menu)
        temp_value = temp_value % 1000;
      else
        temp_value = temp_value % 10000;
    }
    ShowChannel();
    return 1;
  }
  return 0;
}

int freq_pwm_calibrate(printf_func pfunc, int argc, char** argv, void* device_config)
{
  int f1, f2;
  f1 = atoi(argv[0]);
  if (f1 <= 0)
  {
    pfunc("Invalid f1\n");
    return 1;
  }
  f2 = atoi(argv[1]);
  if (f2 <= 0)
  {
    pfunc("Invalid f2\n");
    return 1;
  }
  return 0;
}

static void ChangeDuty(int change)
{
  unsigned int d = channel[current_channel].duty, c;
  if (change > 0)
  {
    d = (d + change) % 1000;
    if (!d)
      d = 1;
  }
  else
  {
    c = -change;
    if (c >= d)
      d = 1;
    else
      d -= c;
  }
  channel[current_channel].duty = d;
}

static void ChangeFrequency(int change)
{
  unsigned int f = channel[current_channel].frequency / frequency_range, c;
  if (change > 0)
  {
    f = (f + change) % 10000;
    switch (frequency_range)
    {
    case 1:
      if (f < MINIMUM_PWM_FREQ)
        f = MINIMUM_PWM_FREQ;
      break;
    default:
      if (!f)
        f = 1;
    }
  }
  else
  {
    c = -change;
    if (c >= f)
    {
      switch (frequency_range)
      {
      case 1:
        f = MINIMUM_PWM_FREQ;
        break;
      default:
        f = 1;
      }
    }
    else
      f -= c;
  }
  channel[current_channel].frequency = f * frequency_range;
}

int freq_pwm_ui_encoder_handler(void *config, int counter, int button_pressed)
{
  if (counter)
  {
    if (default_menu)
    {
      switch (duty_multiplier)
      {
        case 0:
          ChangeDuty(counter);
          break;
        case 1:
          ChangeDuty(counter * 10);
          break;
        case 2:
          ChangeDuty(counter * 100);
          break;
      }
    }
    else
    {
      switch (frequency_multiplier)
      {
        case 0:
          ChangeFrequency(counter);
          break;
        case 1:
          ChangeFrequency(counter * 10);
          break;
        case 2:
          ChangeFrequency(counter * 100);
          break;
        case 3:
          ChangeFrequency(counter * 1000);
          break;
      }
    }
    if (channel[current_channel].enabled)
      pwm_set_freq(current_channel, channel[current_channel].frequency, channel[current_channel].duty);
    ShowChannel();
    return 1;
  }
  return 0;
}