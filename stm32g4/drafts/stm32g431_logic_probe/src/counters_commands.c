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
  pfunc("low %d high %d freq_low %d freq_high %d\n", counter_low, counter_high, counter_freq_low, counter_freq_high);
  return 0;
}

void register_counters_commands(void)
{
  shell_register_command(&counters_command);
}
