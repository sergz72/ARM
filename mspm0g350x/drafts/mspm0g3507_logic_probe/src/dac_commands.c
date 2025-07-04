#include "dac_commands.h"
#include "board.h"
#include <shell.h>
#include <stdlib.h>

#include "dl_comp.h"

static int dac_low_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int dac_low_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem dac_low_command_items[] = {
  {NULL, param_handler, dac_low_handler1},
{NULL, NULL, dac_low_handler2}
};
static const ShellCommand dac_low_command = {
  dac_low_command_items,
  "dac_low",
  NULL
};

static int dac_high_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int dac_high_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem dac_high_command_items[] = {
  {NULL, param_handler, dac_high_handler1},
{NULL, NULL, dac_high_handler2}
};
static const ShellCommand dac_high_command = {
  dac_high_command_items,
  "dac_high",
  NULL
};

static int dac_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int dac_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem dac_command_items[] = {
  {NULL, param_handler, dac_handler1},
{NULL, NULL, dac_handler2}
};
static const ShellCommand dac_command = {
  dac_command_items,
  "dac",
  NULL
};

static unsigned int mv_from_8(unsigned int code)
{
  if (code >= 255)
    return DAC_REFERENCE_VOLTAGE;
  return code * DAC_REFERENCE_VOLTAGE / 256;
}

static int dac_low_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("%d\n", mv_from_8(DL_COMP_getDACCode0(COMP_LOW_INSTANCE)));
  return 0;
}

static int dac_low_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  DL_COMP_setDACCode0(COMP_LOW_INSTANCE, mv_to_8(value));
  return 0;
}

static int dac_high_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("%d\n", mv_from_8(DL_COMP_getDACCode0(COMP_HIGH_INSTANCE)));
  return 0;
}

static int dac_high_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  DL_COMP_setDACCode0(COMP_HIGH_INSTANCE, mv_to_8(value));
  return 0;
}

static int dac_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  //todo
  pfunc("%d\n", 0);//mv_from_8(DAC3->DHR_highR1));
  return 0;
}

static int dac_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  //todo
  int value = atoi(argv[0]);
  //DAC3->DHR_highR1 = mv_to_8(value);
  return 0;
}

void register_dac_commands(void)
{
  shell_register_command(&dac_low_command);
  shell_register_command(&dac_high_command);
  shell_register_command(&dac_command);
}
