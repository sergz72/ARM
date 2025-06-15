#include "dac_commands.h"
#include "board.h"
#include <shell.h>
#include <stdlib.h>

static int dac11_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int dac11_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem dac11_command_items[] = {
  {NULL, param_handler, dac11_handler1},
{NULL, NULL, dac11_handler2}
};
static const ShellCommand dac11_command = {
  dac11_command_items,
  "dac11",
  NULL
};

static int dac12_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int dac12_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem dac12_command_items[] = {
  {NULL, param_handler, dac12_handler1},
{NULL, NULL, dac12_handler2}
};
static const ShellCommand dac12_command = {
  dac12_command_items,
  "dac12",
  NULL
};

static int dac31_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int dac31_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem dac31_command_items[] = {
  {NULL, param_handler, dac31_handler1},
{NULL, NULL, dac31_handler2}
};
static const ShellCommand dac31_command = {
  dac31_command_items,
  "dac31",
  NULL
};

static unsigned int mv_from_12(unsigned int code)
{
  if (code >= 4095)
    return DAC_REFERENCE_VOLTAGE;
  return code * DAC_REFERENCE_VOLTAGE / 4095;
}

static int dac11_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("%d\n", mv_from_12(DAC1->DHR12R1));
  return 0;
}

static int dac11_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  DAC1->DHR12R1 = mv_to_12(value);
  return 0;
}

static int dac12_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("%d\n", mv_from_12(DAC1->DHR12R2));
  return 0;
}

static int dac12_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  DAC1->DHR12R2 = mv_to_12(value);
  return 0;
}

static int dac31_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("%d\n", mv_from_12(DAC3->DHR12R1));
  return 0;
}

static int dac31_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  DAC3->DHR12R1 = mv_to_12(value);
  return 0;
}

void register_dac_commands(void)
{
  shell_register_command(&dac11_command);
  shell_register_command(&dac12_command);
  shell_register_command(&dac31_command);
}
