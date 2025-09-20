#include "board.h"
#include "multimeter_commands.h"
#include <limits.h>
#include <shell.h>
#include <multimeter.h>
#include <stdio.h>
#include <stdlib.h>

static int getr_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem getr_command_items[] = {
  {NULL, NULL, getr_handler}
};
static const ShellCommand getr_command = {
  getr_command_items,
  "getr",
  "getr",
  NULL,
  NULL
};

static int calibrate_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem calibrate_command_items[] = {
  {NULL, param_handler, calibrate_handler},
  {NULL, param_handler, NULL},
  {NULL, NULL, calibrate_handler}
};
static const ShellCommand calibrate_command = {
  calibrate_command_items,
  "calibrate",
  "calibrate [level R]",
  NULL,
  NULL
};

static int getr_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  getr_result value;
  int rc = calculateR(&value, pfunc);
  if (rc)
    return rc;

  if (value.r == LONG_LONG_MAX)
    pfunc("R = infinity, level = %d, uV = %d\r\n", value.level, value.uV);
  else if (value.r < 1000000)
    pfunc("R = %d.%03d Ohm, level = %d, uV = %d", value.r / 1000, value.r % 1000, value.level, value.uV);
  else
    pfunc("R = %d.%06d kOhm, level = %d, uV = %d", value.r / 1000000, value.r % 1000000, value.level, value.uV);

  return 0;
}

void print_calibration_data(printf_func pfunc)
{
  for (int l = 0; l <= MAX_LEVEL; l++)
    pfunc("level %d dac_level %d gain %d r=%d v=%d\r\n", l, dac_levels[l], adc_gains[l], calibration_data->vr[l].r, calibration_data->vr[l].v);
}

static int calibrate_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  if (argc == 0)
  {
    print_calibration_data(pfunc);
    return 0;
  }
  int level = atoi(argv[0]);
  if (level < 0 || level > MAX_LEVEL)
    return 1;
  int R = atoi(argv[1]);
  if (R <= 0)
    return 3;
  int result;
  int rc = calibrateR(level, R, &result);
  if (rc)
    return rc;
  pfunc("result = %d uV\r\n", result);
  return 0;
}

void register_multimeter_commands(void)
{
  shell_register_command(&getr_command);
  shell_register_command(&calibrate_command);
}
