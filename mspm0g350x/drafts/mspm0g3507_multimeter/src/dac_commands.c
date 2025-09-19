#include "dac_commands.h"
#include "board.h"
#include <shell.h>
#include <stdlib.h>
#include <dl_dac12.h>

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

static unsigned int mv_from_12(unsigned int code)
{
  if (code >= 255)
    return DAC_REFERENCE_VOLTAGE;
  return code * DAC_REFERENCE_VOLTAGE / 4096;
}

static int dac_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("%d\n", mv_from_12(DAC0->DATA0));
  return 0;
}

static int dac_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  DL_DAC12_output12(DAC0, mv_to_12(value));
  return 0;
}

void register_dac_commands(void)
{
  shell_register_command(&dac_command);
}
