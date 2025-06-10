#include "board.h"
#include "dac_commands.h"
#include <shell.h>

static int set_frequency_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem set_frequency_command_items[] = {
  {NULL, param_handler, NULL},
{NULL, NULL, set_frequency_handler}
};
static const ShellCommand set_frequency_command = {
  set_frequency_command_items,
  "dac_frequency",
  NULL
};

static int set_volume_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem set_volume_command_items[] = {
  {NULL, param_handler, NULL},
{NULL, NULL, set_volume_handler}
};
static const ShellCommand set_volume_command = {
  set_volume_command_items,
  "dac_volume",
  NULL
};

static int set_frequency_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return 0;
}

static int set_volume_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return 0;
}

void register_dac_commands(void)
{
  shell_register_command(&set_frequency_command);
  shell_register_command(&set_volume_command);
}
