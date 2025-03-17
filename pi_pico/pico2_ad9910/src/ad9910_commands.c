#include "board.h"
#include "ad9910_commands.h"
#include <shell.h>
#include <stdlib.h>
#include <ad9910.h>

static int freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem freq_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, freq_handler}
};
static const ShellCommand freq_command = {
  freq_command_items,
  "ad9910_freq_amp",
  "ad9910_freq_amp channel frequency amplitude"
};

static int power_down_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem power_down_command_items[] = {
    {NULL, param_handler, NULL},
  {NULL, NULL, power_down_handler}
};
static const ShellCommand power_down_command = {
  power_down_command_items,
  "ad9910_power_down",
  "ad9910_power_down channel"
};

static int power_up_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem power_up_command_items[] = {
    {NULL, param_handler, NULL},
  {NULL, NULL, power_up_handler}
};
static const ShellCommand power_up_command = {
  power_up_command_items,
  "ad9910_power_up",
  "ad9910_power_up channel"
};

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init_command_items[] = {
  {NULL, param_handler, NULL},
{NULL, NULL, init_handler}
};
static const ShellCommand init_command = {
  init_command_items,
  "ad9910_init",
  "ad9910_init channel"
};

static const AD9910_Config config =
{
  .refclk = AD9910_REFCLK_FREQUENCY,
  .pllclk = AD9910_PLL_FREQUENCY,
  .charge_pump_current_ua = 312
};

static unsigned long calculateFrequencyCode(unsigned long frequency)
{
  return (unsigned long)(((unsigned long long int)frequency << AD9910_ACCUMULATOR_BITS) / AD9910_PLL_FREQUENCY);
}

static int freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel, freq, amp;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= AD9910_MAX_CHANNELS)
  {
    pfunc("Invalid channel\n");
    return 1;
  }
  freq = atoi(argv[1]);
  if (freq < 0 || freq > 400000000)
  {
    pfunc("Invalid frequency\n");
    return 2;
  }
  amp = atoi(argv[2]);
  if (amp < 0 || amp > 0x3FFF)
  {
    pfunc("Invalid amplitude\n");
    return 2;
  }


  unsigned long code = calculateFrequencyCode(freq);
  ad9910_set_freq_phase_amplitude(channel, code, 0, amp);

  return 0;
}

static int power_down_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= AD9910_MAX_CHANNELS)
  {
    pfunc("Invalid channel\n");
    return 1;
  }

  ad9910_power_down(channel);

  return 0;
}

static int power_up_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= AD9910_MAX_CHANNELS)
  {
    pfunc("Invalid channel\n");
    return 1;
  }

  ad9910_power_up(channel);

  return 0;
}

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= AD9910_MAX_CHANNELS)
  {
    pfunc("Invalid channel\n");
    return 1;
  }

  return ad9910_init(channel, &config);
}

void register_ad9910_commands(void)
{
  shell_register_command(&freq_command);
  shell_register_command(&power_down_command);
  shell_register_command(&power_up_command);
  shell_register_command(&init_command);
}
