#include "dac_commands.h"
#include "board.h"
#include <shell.h>
#include <stdlib.h>

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

#ifdef DAC3_PRESENT
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
#endif

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

static int dac_low_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("%d\n", get_l_voltage());
  return 0;
}

static int dac_low_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  set_l_voltage(value);
  return 0;
}

#ifdef DAC3_PRESENT
static int dac_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("%d\n", get_dac_voltage());
  return 0;
}

static int dac_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  set_dac_voltage(value);
  return 0;
}
#endif

static int dac_high_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("%d\n", get_h_voltage());
  return 0;
}

static int dac_high_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[0]);
  set_h_voltage(value);
  return 0;
}

void register_dac_commands(void)
{
  shell_register_command(&dac_low_command);
#ifdef DAC3_PRESENT
  shell_register_command(&dac_command);
#endif
  shell_register_command(&dac_high_command);
}
