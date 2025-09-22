#include "board.h"
#include "ads1220_commands.h"
#include <ads1220.h>
#include <shell.h>
#include <stdlib.h>

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init_command_items[] = {
  {NULL, NULL, init_handler}
};
static const ShellCommand init_command = {
  init_command_items,
  "ads1220_init",
  "ads1220_init",
  NULL,
  NULL
};

static int reset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem reset_command_items[] = {
  {NULL, NULL, reset_handler}
};
static const ShellCommand reset_command = {
  reset_command_items,
  "ads1220_reset",
  "ads1220_reset",
  NULL,
  NULL
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  { NULL, param_handler, NULL},
  { NULL, param_handler, NULL},
  {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "ads1220_read",
  "ads1220_read channel gain",
  NULL,
  NULL
};

static int read_temp_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_temp_command_items[] = {
  {NULL, NULL, read_temp_handler}
};
static const ShellCommand read_temp_command = {
  read_temp_command_items,
  "ads1220_read_temp",
  "ads1220_read_temp",
  NULL,
  NULL
};

static int set_idac_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem set_idac_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, set_idac_handler}
};
static const ShellCommand set_idac_command = {
  set_idac_command_items,
  "ads1220_set_idac",
  "ads1220_set_idac current mux1 mux2",
  NULL,
  NULL
};

static int read_register_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_register_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, read_register_handler}
};
static const ShellCommand read_register_command = {
  read_register_command_items,
  "ads1220_read_register",
  "ads1220_read_register reg_no",
  NULL,
  NULL
};

static int write_register_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem write_register_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, write_register_handler}
};
static const ShellCommand write_register_command = {
  write_register_command_items,
  "ads1220_write_register",
  "ads1220_write_register reg_no value",
  NULL,
  NULL
};

static int calibrate_offset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem calibrate_offset_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, calibrate_offset_handler}
};
static const ShellCommand calibrate_offset_command = {
  calibrate_offset_command_items,
  "ads1220_cal_offset",
  "ads1220_cal_offset channel",
  NULL,
  NULL
};

static const ads1220_configuration ads1220_config =
{
  .burnout_sources_enable = 0,
  .continuous_conversion_mode = 0,
  .data_rate = ADS1220_DATA_RATE_20,
  .temperature_sensor_mode = 0,
  .mode = ADS1220_MODE_NORMAL,
  .pga_bypass = 0,
  .auto_close_low_side_switch = 0,
  .data_ready_on_dout = 0,
  .filter_50_60 = ADS1220_FILTER_50,
  .gain = ADS1220_PGA_GAIN_1,
  .idac1_mux = ADS1220_IDAC_MUX_OFF,
  .idac2_mux = ADS1220_IDAC_MUX_OFF,
  .idac_current = ADS1220_IDAC_OFF,
  .mux = ADS1220_MUX_AIN0_AIN1,
  .vref = ADS1220_VREF_INTERNAL
};

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  set_cpha1();
  ads1220_set_configuration(0, &ads1220_config);
  return 0;
}

static int reset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  ads1220_reset(0);
  return 0;
}

static int read_voltage_func(printf_func pfunc, unsigned char channel, unsigned char gain)
{
  ads1220_read_voltage_configuration configuration =
  {
    .channel = 0,
    .channel_no = channel,
    .gain = gain,
    .pga_bypass = gain < 3,
    .vref = ADS1220_VREF,
    .input_divider_coefficient = ADS1220_INPUT_COEFFICIENT
  };
  ads1220_data result;
  int rc = ads1220_read_voltage(&configuration, &result, ADS1220_TIMEOUT);
  if (rc)
    return rc;
  pfunc("code = %08X, result = %f V\r\n", result.value, result.voltage);
  return 0;
}

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = atoi(argv[0]);
  if (channel < 0 || channel >= 15)
    return 1;
  int gain = atoi(argv[1]);
  if (gain < 0 || gain > 7)
    return 2;
  return read_voltage_func(pfunc, channel, gain);
}

static int read_temp_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  ads1220_data result;
  int rc = ads1220_read_temperature(0, &result, ADS1220_TIMEOUT);
  if (rc)
    return rc;
  pfunc("code = %08X, result = %f C\r\n", result.value, result.voltage);
  return 0;
}

static int set_idac_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int idac = atoi(argv[0]);
  if (idac < 0 || idac > 7)
    return 1;
  int i1mux = atoi(argv[1]);
  if (i1mux < 0 || i1mux >= 7)
    return 2;
  int i2mux = atoi(argv[2]);
  if (i2mux < 0 || i2mux >= 7)
    return 3;
  ads1220_set_idac(0, idac, i1mux, i2mux);
  return 0;
}

static int read_register_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int reg_no = atoi(argv[0]);
  if (reg_no < 0 || reg_no > 3)
    return 1;
  unsigned char reg_data;
  ads1220_read_registers(0, reg_no, &reg_data, 1);
  pfunc("%02X\r\n", reg_data);
  return 0;
}

static int write_register_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int reg_no = atoi(argv[0]);
  if (reg_no < 0 || reg_no > 3)
    return 1;
  unsigned long reg_data = strtoul(argv[1], NULL, 16);
  ads1220_write_registers(0, reg_no, (unsigned char*)&reg_data, 1);
  return 0;
}

static int calibrate_offset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = atoi(argv[0]);
  if (channel < 0 || channel >= 15)
    return 1;
  int rc = ads1220_calibrate_offset(0, channel, 10);
  if (rc)
    return rc;
  for (int gain = 0; gain <= ADS1220_MAX_GAIN; gain++)
    pfunc("gain %d offset %08X\r\n", gain, ads1220_offsets[0][channel][gain]);
  return 0;
}

void register_ads1220_commands(void)
{
  shell_register_command(&init_command);
  shell_register_command(&reset_command);
  shell_register_command(&read_command);
  shell_register_command(&read_temp_command);
  shell_register_command(&set_idac_command);
  shell_register_command(&read_register_command);
  shell_register_command(&write_register_command);
  shell_register_command(&calibrate_offset_command);
}
