#include "board.h"
#include "multimeter_commands.h"
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
  {NULL, param_handler, NULL},
  {NULL, NULL, calibrate_handler}
};
static const ShellCommand calibrate_command = {
  calibrate_command_items,
  "calibrate",
  "calibrate [level gain R]",
  NULL,
  NULL
};

static int getr_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned long long int value;
  int rc = calculateR(&value);
  if (rc)
    return rc;

  if (value < 1000000)
    pfunc("R = %d.%03d Ohm", value / 1000, value % 1000);
  else
    pfunc("R = %d.%06d kOhm", value / 1000000, value % 1000000);

  return 0;
}

void print_calibration_data(printf_func pfunc)
{
  for (int l = 0; l <= DAC_MAX_LEVEL; l++)
  {
    for (int g = 0; g <= ADC_MAX_GAIN; g++)
      pfunc("level %d gain %d r=%d v=%d\r\n", l, g, calibration_data->r[l][g], calibration_data->v[l][g]);
  }
}

static int calibrate_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  if (argc == 0)
  {
    print_calibration_data(pfunc);
    return 0;
  }
  int level = atoi(argv[0]);
  if (level < 0 || level > DAC_MAX_LEVEL)
    return 1;
  int gain = atoi(argv[1]);
  if (gain < 0 || gain > ADC_MAX_GAIN)
    return 2;
  int R = atoi(argv[2]);
  if (R <= 0)
    return 3;
  int result;
  int rc = calibrateR(level, gain, R, &result);
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
