#include <malloc.h>
#include "dev_si5351.h"
#include <si5351.h>
#include "ui.h"
#include "board.h"
#include "device_config.h"
#include "dev_keyboard.h"
#include <stdlib.h>

#define MAX_DIVIDER 128
#define MINIMUM_FREQUENCY 1000000

#define isValidCorr(x) (x >= -50000 && x <= 50000)

typedef struct {
  int corr;
} SI5351Config;

typedef struct {
  unsigned int frequency;
  unsigned int divider;
  int enabled;
} DDS_Channel;

static int current_channel;
static DDS_Channel channel[3];
static unsigned int temp_value;

static void ChannelInit(int c)
{
  channel[c].frequency = DEFAULT_SI5351_FREQ;
  channel[c].divider = 1;
  channel[c].enabled = 0;
}

void* si5351_initializer(void)
{
  if (!si5351_write(0, SI5351_CRYSTAL_LOAD, SI5351_CRYSTAL_LOAD_8PF))
  {
    si5351_init();

    SI5351Config* cfg = malloc(sizeof(SI5351Config));
    if (cfg)
    {
      current_channel = 0;
      ChannelInit(0);
      ChannelInit(1);
      ChannelInit(2);

      if (read_config_from_eeprom(cfg, sizeof(SI5351Config)) || !isValidCorr(cfg->corr))
        cfg->corr = 0;
      si5351_set_correction(0, cfg->corr);
    }
    return cfg;
  }
  return NULL;
}

static void ShowMenu(void)
{
  LED_Printf(3, 0, "C%d%sD%3d", current_channel + 1, channel[current_channel].enabled ? "DI" : "En", channel[current_channel].divider);
}

static void ShowChannel(int c)
{
  unsigned int f = (c == current_channel && cursorEnabled) ? temp_value : channel[c].frequency;
  LED_Printf(c, 0, "%8d", f);
}

void si5351_ui_init_handler(void* config)
{
  ShowChannel(0);
  ShowChannel(1);
  ShowChannel(2);
  ShowMenu();
}

static void ToggleChannelOutput(void)
{
  int enabled = channel[current_channel].enabled ? 0 : 1;
  channel[current_channel].enabled = enabled;
  if (enabled)
  {
    si5351_set_freq(0, channel[current_channel].frequency, -900000000, current_channel, channel[current_channel].divider);
    si5351_clock_enable(0, current_channel, 1);
  }
  else
    si5351_clock_enable(0, current_channel, 0);
}

static void SelectNextDivider(void)
{
  unsigned int d = channel[current_channel].divider;
  d = d == MAX_DIVIDER ? 1 : d << 1;
  channel[current_channel].divider = d;
  if (channel[current_channel].enabled)
    si5351_set_freq(0, channel[current_channel].frequency, -900000000, current_channel, d);
}

static void DisableChannel(int c)
{
  if (channel[c].enabled)
  {
    channel[c].enabled = 0;
    si5351_clock_enable(0, c, 0);
  }
}

int si5351_ui_keyboard_handler(void *config, unsigned int event)
{
  switch (event)
  {
  case KEYBOARD_EVENT_ENTER:
    if (cursorEnabled)
    {
      if (temp_value < MINIMUM_FREQUENCY)
        temp_value = MINIMUM_FREQUENCY;
      channel[current_channel].frequency = temp_value;
      if (channel[current_channel].enabled)
        si5351_set_freq(0, temp_value, -900000000, current_channel, channel[current_channel].divider);
      cursorEnabled = 0;
      ShowChannel(current_channel);
      return 1;
    }
    break;
  case KEYBOARD_EVENT_LEAVE:
    if (cursorEnabled)
    {
      cursorEnabled = 0;
      ShowChannel(current_channel);
      return 1;
    }
    DisableChannel(0);
    DisableChannel(1);
    DisableChannel(2);
    break;
  case KEYBOARD_EVENT_F1:
    if (!cursorEnabled)
    {
      current_channel++;
      if (current_channel == 3)
        current_channel = 0;
      ShowMenu();
      return 1;
    }
    break;
  case KEYBOARD_EVENT_F2:
    if (!cursorEnabled)
    {
      ToggleChannelOutput();
      ShowMenu();
      return 1;
    }
    break;
  case KEYBOARD_EVENT_F3:
    if (!cursorEnabled)
    {
      SelectNextDivider();
      ShowMenu();
      return 1;
    }
    break;
  case KEYBOARD_EVENT_F4:
    break;
  default:
    if (!cursorEnabled)
    {
      enableCursor(current_channel, 0, 7);
      temp_value = event - 1;
    }
    else
    {
      temp_value = temp_value * 10 + event - 1;
      temp_value = temp_value % 100000000;
    }
    ShowChannel(current_channel);
    return 1;
  }
  return 0;
}

int si5351_print_config(printf_func pfunc, void* config)
{
  SI5351Config* cfg = (SI5351Config*)config;
  if (cfg)
  {
    pfunc("corr %d\n", cfg->corr);
    return 0;
  }
  pfunc("NULL config\n");
  return 1;
}

int si5351_set_config(printf_func pfunc, int argc, char** argv, void* config)
{
  SI5351Config* cfg = (SI5351Config*)config;
  int corr;
  if (cfg)
  {
    corr = atoi(argv[0]);
    if (!isValidCorr(corr))
    {
      pfunc("Invalid corr\n");
      return 1;
    }
    cfg->corr = corr;
    pfunc("corr %d\n", cfg->corr);
    return write_config_to_eeprom(cfg, sizeof(SI5351Config));
  }
  pfunc("NULL config\n");
  return 1;
}
