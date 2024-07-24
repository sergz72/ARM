#include "ad9850_commands.h"
#include "board.h"
#include <shell.h>
#include <ad9850.h>
#include <stdlib.h>
#include <string.h>

static int ad9850freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem ad9850freq_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, ad9850freq_handler}
};
static const ShellCommand ad9850freq_command = {
    ad9850freq_command_items,
    "ad9850freq",
    "ad9850freq frequency"
};

static int ad9850enable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem ad9850enable_command_items[] = {
    {NULL, NULL, ad9850enable_handler}
};
static const ShellCommand ad9850enable_command = {
    ad9850enable_command_items,
    "ad9850enable",
    NULL
};

static int ad9850disable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem ad9850disable_command_items[] = {
    {NULL, NULL, ad9850disable_handler}
};
static const ShellCommand ad9850disable_command = {
    ad9850disable_command_items,
    "ad9850disable",
    NULL
};

static int ad9850freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned int code;
  int freq = atoi(argv[0]);
  if (freq > 0 && freq <= AD9850_MCLK_FREQ / 2)
  {
    code = (unsigned int)(((unsigned long long int)freq << AD9850_ACCUMULATOR_BITS) / AD9850_MCLK_FREQ);
    pfunc("freq = %d, freq word = %d\n", freq, code);
    ad9850_set_freq_word(0, code, 0);
    return 0;
  }
  pfunc("Invalid frequency\n");
  return 1;
}

static int ad9850enable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  ad9850_power_up(0);
  return 0;
}

static int ad9850disable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  ad9850_power_down(0);
  return 0;
}

void register_ad9850_commands(void)
{
  ad9850_init();
  shell_register_command(&ad9850freq_command);
  shell_register_command(&ad9850enable_command);
  shell_register_command(&ad9850disable_command);
}
