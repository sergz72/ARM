#include "dac_commands.h"
#include "board.h"
#include <shell.h>
#include <stdlib.h>

static int dac_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int dac_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem dac_command_items[] = {
  {NULL, param_handler, dac_handler1},
{NULL, NULL, dac_handler2}
};
static const ShellCommand dac_command = {
  dac_command_items,
  "dac",
  "dac [value]",
  NULL,
  NULL
};

static int dac_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("%d\n", dac_get());
  return 0;
}

static int dac_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  dac_set(value);
  return 0;
}

void register_dac_commands(void)
{
  shell_register_command(&dac_command);
}
