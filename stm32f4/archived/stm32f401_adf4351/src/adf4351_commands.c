#include "adf4351_commands.h"
#include "board.h"
#include <shell.h>
#include <adf4351.h>
#include <stdlib.h>
#include <string.h>

static int adf4351freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem adf4351freq_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, adf4351freq_handler}
};
static const ShellCommand adf4351freq_command = {
    adf4351freq_command_items,
    "adf4351freq",
    "adf4351freq frequency"
};

static int adf4351enable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem adf4351enable_command_items[] = {
    {NULL, NULL, adf4351enable_handler}
};
static const ShellCommand adf4351enable_command = {
    adf4351enable_command_items,
    "adf4351enable",
    NULL
};

static int adf4351disable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem adf4351disable_command_items[] = {
    {NULL, NULL, adf4351disable_handler}
};
static const ShellCommand adf4351disable_command = {
    adf4351disable_command_items,
    "adf4351disable",
    NULL
};

static int adf4351freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned int code;
  int freq = atoi(argv[0]);
  if (freq > 0 && freq <= adf4351_MCLK_FREQ / 2)
  {
    code = (unsigned int)(((unsigned long long int)freq << adf4351_ACCUMULATOR_BITS) / adf4351_MCLK_FREQ);
    pfunc("freq = %d, freq word = %d\n", freq, code);
    adf4351_set_freq_word(0, code, 0);
    return 0;
  }
  pfunc("Invalid frequency\n");
  return 1;
}

static int adf4351enable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  adf4351_power_up(0);
  return 0;
}

static int adf4351disable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  adf4351_power_down(0);
  return 0;
}

void register_adf4351_commands(void)
{
  adf4351_init();
  shell_register_command(&adf4351freq_command);
  shell_register_command(&adf4351enable_command);
  shell_register_command(&adf4351disable_command);
}
