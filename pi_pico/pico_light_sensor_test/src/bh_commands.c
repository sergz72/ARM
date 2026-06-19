#include "board.h"
#include <bh_commands.h>
#include <shell.h>
#include <bh1750.h>
#include <stdlib.h>

static int bh_init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem bh_init_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, bh_init_handler}
};
static const ShellCommand bh_init_command = {
  bh_init_command_items,
  "bh_init",
  "bh_init mtreg_value",
  NULL,
  NULL
};

static int bh_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem bh_get_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, bh_get_handler}
};
static const ShellCommand bh_get_command = {
  bh_get_command_items,
  "bh_get",
  "bh_get coefficient",
  NULL,
  NULL
};

static int bh_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  float coefficient = atof(argv[0]);
  unsigned short result;
  unsigned int rc = bh1750_measure(BH1750_ONE_TIME_HIGH_RES_MODE_2, &result);
  if (rc)
    return (int)rc;
  float luminocity = (float)result * coefficient;
  pfunc("luminocity: %f\n", luminocity);
  return 0;
}

static int bh_init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int mtreg = atoi(argv[0]);
  if (mtreg < 0 || mtreg > BH1750_MTREG_MAX)
  {
    pfunc("invalid mtreg value\r\n");
    return 1;
  }
  return bh1750_set_measurement_time(mtreg);
}

void register_bh_commands(void)
{
  shell_register_command(&bh_init_command);
  shell_register_command(&bh_get_command);
}
