#include "counters_commands.h"
#include "board.h"
#include <shell.h>
#include <stdlib.h>

static int counters_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem counters_command_items[] = {
{NULL, NULL, counters_handler}
};
static const ShellCommand counters_command = {
  counters_command_items,
  "counters",
  NULL
};

static int counters_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("low %lu high %lu z %lu freq_low %lu freq_high %lu freq_rs %lu\n",
        counter_low, counter_high, counter_z,
        counter_freq_low, counter_freq_high, counter_freq_rs);
  return 0;
}

void register_counters_commands(void)
{
  shell_register_command(&counters_command);
}
