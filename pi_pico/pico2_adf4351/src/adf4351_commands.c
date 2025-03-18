#include "board.h"
#include "adf4351_commands.h"
#include <shell.h>
#include <stdlib.h>
#include <adf4351.h>

#define R_COUNTER_BASE 1

static const ADF4351Config config =
{
  .refin = 10000000,
  .reference_divide_by_2 = 1,
  .reference_doubler = 0,
  .cycle_slip_reduction = 0,
  .low_spur_mode = 0,
  .r_counter = R_COUNTER_BASE,
  .mod = 100,
  .pd_polarity_positive = 1,
  .charge_pump_current = 5000 // uA
};

static int freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem freq_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, freq_handler}
};
static const ShellCommand freq_command = {
  freq_command_items,
  "adf4351_freq",
  "adf4351_freq channel frequency"
};

static int power_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem power_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, power_handler}
};
static const ShellCommand power_command = {
  power_command_items,
  "adf4351_power",
  "adf4351_power channel power"
};

static int power_down_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem power_down_command_items[] = {
    {NULL, param_handler, NULL},
  {NULL, NULL, power_down_handler}
};
static const ShellCommand power_down_command = {
  power_down_command_items,
  "adf4351_power_down",
  "adf4351_power_down channel"
};

static int power_up_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem power_up_command_items[] = {
    {NULL, param_handler, NULL},
  {NULL, NULL, power_up_handler}
};
static const ShellCommand power_up_command = {
  power_up_command_items,
  "adf4351_power_up",
  "adf4351_power_up channel"
};

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init_command_items[] = {
  {NULL, param_handler, NULL},
{NULL, NULL, init_handler}
};
static const ShellCommand init_command = {
  init_command_items,
  "adf4351_init",
  "adf4351_init channel"
};

static int freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel;
  long long int freq;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= ADF4351_MAX_CHANNELS)
  {
    pfunc("Invalid channel\n");
    return 1;
  }
  freq = atoll(argv[1]);
  if (freq < ADF4351_MIN_FREQUENCY || freq > ADF4351_MAX_FREQUENCY)
  {
    pfunc("Invalid frequency\n");
    return 2;
  }


  adf4351_set_freq(channel, freq);

  return 0;
}

static int power_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel, power;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= ADF4351_MAX_CHANNELS)
  {
    pfunc("Invalid channel\n");
    return 1;
  }
  power = atoi(argv[1]);
  if (power < 0 || power > 3)
  {
    pfunc("Invalid power\n");
    return 2;
  }


  adf4351_set_rf_power(channel, 0, power);

  return 0;
}

static int power_down_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= ADF4351_MAX_CHANNELS)
  {
    pfunc("Invalid channel\n");
    return 1;
  }

  adf4351_power_down(channel);

  return 0;
}

static int power_up_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= ADF4351_MAX_CHANNELS)
  {
    pfunc("Invalid channel\n");
    return 1;
  }

  adf4351_power_up(channel);

  return 0;
}

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= ADF4351_MAX_CHANNELS)
  {
    pfunc("Invalid channel\n");
    return 1;
  }

  adf4351_init(channel, &config);
  adf4351_output_enable(0, 0, 1);

  return 0;
}

void register_adf4351_commands(void)
{
  shell_register_command(&freq_command);
  shell_register_command(&power_command);
  shell_register_command(&power_down_command);
  shell_register_command(&power_up_command);
  shell_register_command(&init_command);
}
