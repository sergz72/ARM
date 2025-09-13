#include "board.h"
#include <ads1259.h>
#include "ads1259_commands.h"
#include <shell.h>
#include <stdlib.h>

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init_command_items[] = {
  {NULL, NULL, init_handler}
};
static const ShellCommand init_command = {
  init_command_items,
  "ads1259_init",
  "ads1259_init",
  NULL,
  NULL
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "ads1259_read",
  "ads1259_read",
  NULL,
  NULL
};

static int calibrate_offset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem calibrate_offset_command_items[] = {
  {NULL, NULL, calibrate_offset_handler}
};
static const ShellCommand calibrate_offset_command = {
  calibrate_offset_command_items,
  "ads1259_cal_offset",
  "ads1259_cal_offset",
  NULL,
  NULL
};

static int calibrate_offset2_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem calibrate_offset2_command_items[] = {
  {NULL, NULL, calibrate_offset2_handler}
};
static const ShellCommand calibrate_offset2_command = {
  calibrate_offset2_command_items,
  "ads1259_cal_offset2",
  "ads1259_cal_offset2",
  NULL,
  NULL
};

static int calibrate_gain_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem calibrate_gain_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, calibrate_gain_handler}
};
static const ShellCommand calibrate_gain_command = {
  calibrate_gain_command_items,
  "ads1259_cal_gain",
  "ads1259_cal_gain value",
  NULL,
  NULL
};

static int set_fsc_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem set_fsc_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, set_fsc_handler}
};
static const ShellCommand set_fsc_command = {
  set_fsc_command_items,
  "ads1259_set_fsc",
  "ads1259_set_fsc value",
  NULL,
  NULL
};

const ads1259_configuration ads1259_config =
{
  .conversion_delay = ADS1259_CONVERSION_DELAY_0,
  .data_rate = ADS1259_DATA_RATE_10,
  .digital_filter_sinc2 = 0,
  .external_reference_enable = 0,
  .internal_reference_bias_enable = 1,
  .out_of_range_flag_enable = 0,
  .pulse_mode = 1,
  .spi_timeout_enable = 0,
  .syncout_enable = 0,
  .checksum_enable = 0
};

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  set_cpha1();
  ads1259_init(0, &ads1259_config);
  return 0;
}

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  double value;
  int rc = ads1259_read_voltage(0, ADS1259_INPUT_COEFFICIENT, ADS1259_VREF, &value, ADS1259_TIMEOUT);
  if (rc)
    return rc;
  pfunc("result = %f\r\n", value);
  return 0;
}

static int calibrate_offset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int rc = ads1259_calibrate_offset(0, 1000, ADS1259_TIMEOUT);
  if (rc)
  {
    pfunc("calibrate offset returned error %d\r\n", rc);
    return rc;
  }
  int value;
  rc = ads1259_read(0, &value, ADS1259_TIMEOUT);
  if (rc)
  {
    pfunc("read returned error %d\r\n", rc);
    return rc;
  }
  pfunc("result = %d\r\n", value);
  return 0;
}

static int calibrate_offset2_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int rc = ads1259_calibrate_offset2(0, ADS1259_TIMEOUT);
  if (rc)
  {
    pfunc("calibrate offset returned error %d\r\n", rc);
    return rc;
  }
  int value;
  rc = ads1259_read(0, &value, ADS1259_TIMEOUT);
  if (rc)
  {
    pfunc("read returned error %d\r\n", rc);
    return rc;
  }
  pfunc("result = %d\r\n", value);
  return 0;
}

static int calibrate_gain_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  ads1259_gain_calibration calibration;
  double value = atof(argv[0]);
  calibration.channel = 0;
  calibration.expected_voltage = value;
  calibration.num_loops = 10;
  calibration.input_divider_coefficient = ADS1259_INPUT_COEFFICIENT;
  calibration.vref = ADS1259_VREF;
  int rc = ads1259_calibrate_gain(&calibration, ADS1259_TIMEOUT);
  pfunc("return code = %d, fsc = %d, expected_value = %d, result = %d\r\n", rc, calibration.fsc, calibration.expected_value, calibration.result);
  rc = ads1259_read_voltage(0, ADS1259_INPUT_COEFFICIENT, ADS1259_VREF, &value, ADS1259_TIMEOUT);
  if (rc)
    return rc;
  pfunc("voltage = %f\r\n", value);
  return 0;
}

static int set_fsc_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int fsc = atoi(argv[0]);
  ads1259_set_fsc(0, fsc);
  return 0;
}

void register_ads1259_commands(void)
{
  shell_register_command(&init_command);
  shell_register_command(&read_command);
  shell_register_command(&calibrate_offset_command);
  shell_register_command(&calibrate_offset2_command);
  shell_register_command(&calibrate_gain_command);
  shell_register_command(&set_fsc_command);
}
