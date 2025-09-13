#include "board.h"
#include "ad7124_commands.h"
#include <ad7124.h>
#include <shell.h>
#include <stdlib.h>
#include <string.h>

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init_command_items[] = {
  {NULL, NULL, init_handler}
};
static const ShellCommand init_command = {
  init_command_items,
  "ad7124_init",
  "ad7124_init",
  NULL,
  NULL
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "ad7124_read",
  "ad7124_read",
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
  "ad7124_cal_offset",
  "ad7124_cal_offset internal|system",
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
  "ad7124_cal_gain",
  "ad7124_cal_gain internal|system|value",
  NULL,
  NULL
};

static int gain_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem gain_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, gain_handler},
  {NULL, NULL, gain_handler}
};
static const ShellCommand gain_command = {
  gain_command_items,
  "ad7124_gain",
  "ad7124_gain channel [value]",
  NULL,
  NULL
};

static int offset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem offset_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, offset_handler},
  {NULL, NULL, offset_handler}
};
static const ShellCommand offset_command = {
  offset_command_items,
  "ad7124_offset",
  "ad7124_offset channel [value]",
  NULL,
  NULL
};

static int channel_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem channel_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, channel_handler}
};
static const ShellCommand channel_command = {
  channel_command_items,
  "ad7124_channel",
  "ad7124_channel channel ainp ainm",
  NULL,
  NULL
};

static int read_register_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_register_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, read_register_handler}
};
static const ShellCommand read_register_command = {
  read_register_command_items,
  "ad7124_read_register",
  "ad7124_read_register read_register reg_no length",
  NULL,
  NULL
};

const ad7124_channel_configuration ad7124_channel_config =
{
  .enable = 1,
  .setup_no = 0,
  .ainm = 1,
  .ainp = 0
};

const ad7124_configuration ad7124_config =
{
  .bipolar = 1,
  .burnout = AD7124_BURNOUT_OFF,
  .ref_bufp = 1,
  .ref_bufm = 1,
  .ain_bufp = 1,
  .ain_bufm = 1,
  .refsel = AD7124_REFSEL_INTERNAL,
  .pga = AD7124_PGA_GAIN_1,
  .filter_type = AD7124_FILTER_SINC4,
  .filter_rej60 = 1,
  .post_filter = AD7124_POST_FILTER_92DB,
  .filter_fs = 4,
  .filter_single_cycle = 0
};

const ad7124_adc_control ad7124_adc_config =
{
  .dout_rdy_del = 0,
  .cont_read = 0,
  .cs_en = 0,
  .data_status = 0,
  .ref_en = 0,
  .power_mode = AD7124_FULL_POWER,
  .mode = AD7124_MODE_IDLE,
  .clk_sel = AD7124_CLOCK_INTERNAL_NO_CLK
};

const ad7124_io_control ad7124_io_config =
{
  .gpio_ctrl = 0,
  .gpio_dat = 0,
  .iout0 = AD7124_IOUT_OFF,
  .iout1 = AD7124_IOUT_OFF,
  .iout0_ch = 0,
  .iout1_ch = 0,
  .vbias = 2, // bias is on ain1
  .pdsw = 0
};

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  set_cpol1cpha1();
  ad7124_setup_adc_control(0, &ad7124_adc_config);
  ad7124_setup_io_control(0, &ad7124_io_config);
  ad7124_setup_configuration(0, 0, &ad7124_config);
  ad7124_setup_channel(0, 0, &ad7124_channel_config);
  return 0;
}

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int ivalue;
  int rc = ad7124_read(0, &ivalue, AD7124_TIMEOUT);
  if (rc)
    return rc;
  double value;
  rc = ad7124_read_voltage(0, AD7124_INPUT_COEFFICIENT, AD7124_VREF, &value, AD7124_TIMEOUT);
  if (rc)
    return rc;
  pfunc("code = %08X, result = %f\r\n", ivalue, value);
  return 0;
}

static int calibrate_offset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int rc;
  if (!strcmp(argv[0], "internal"))
    rc = ad7124_calibrate_offset_internal(0, AD7124_TIMEOUT);
  else if (!strcmp(argv[0], "system"))
    rc = ad7124_calibrate_offset_system(0, AD7124_TIMEOUT);
  else
    return 1;
  if (rc)
  {
    pfunc("calibrate offset returned error %d\r\n", rc);
    return rc;
  }
  int value;
  rc = ad7124_read(0, &value, AD7124_TIMEOUT);
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
  int rc;
  if (!strcmp(argv[0], "internal"))
    rc = ad7124_calibrate_gain_internal(0, AD7124_TIMEOUT);
  else if (!strcmp(argv[0], "system"))
    rc = ad7124_calibrate_gain_system(0, AD7124_TIMEOUT);
  else
    return 1;
  if (rc)
  {
    pfunc("calibrate gain returned error %d\r\n", rc);
    return rc;
  }
  double value;
  rc = ad7124_read_voltage(0, AD7124_INPUT_COEFFICIENT, AD7124_VREF, &value, AD7124_TIMEOUT);
  if (rc)
    return rc;
  pfunc("voltage = %f\r\n", value);
  return 0;
}

static int gain_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = atoi(argv[0]);
  if (channel < 0 || channel > 7)
    return 1;
  if (argc == 1)
  {
    int gain = ad7124_get_gain(0, channel);
    pfunc("gain = %08X\r\n", gain);
  }
  else
  {
    int gain = strtoul(argv[1], NULL, 16);
    ad7124_set_gain(0, channel, gain);
  }
  return 0;
}

static int offset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = atoi(argv[0]);
  if (channel < 0 || channel > 7)
    return 1;
  if (argc == 1)
  {
    int offset = ad7124_get_offset(0, channel);
    pfunc("offset = %08X\r\n", offset);
  }
  else
  {
    int offset = strtoul(argv[1], NULL, 16);
    ad7124_set_offset(0, channel, offset);
  }
  return 0;
}

static int channel_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = atoi(argv[0]);
  if (channel < 0 || channel > 7)
    return 1;
  int ainp = atoi(argv[1]);
  if (ainp < 0 || ainp > 31)
    return 2;
  int ainm = atoi(argv[2]);
  if (ainm < 0 || ainm > 31)
    return 2;
  ad7124_channel_configuration config;
  config.enable = 1;
  config.setup_no = 0;
  config.ainm = ainm;
  config.ainp = ainp;
  ad7124_setup_channel(0, channel, &config);
  return 0; 
}

static int read_register_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int reg_no = atoi(argv[0]);
  if (reg_no < 0 || reg_no > 63)
    return 1;
  int length = atoi(argv[1]);
  if (length < 1 || length > 3)
    return 2;
  unsigned char reg_data[3];
  ad7124_read_register(0, reg_no, reg_data, length);
  for (int i = 0; i < length; i++)
    pfunc("%02X ", reg_data[i]);
  pfunc("\r\n");
  return 0;
}

void register_ad7124_commands(void)
{
  shell_register_command(&init_command);
  shell_register_command(&read_command);
  shell_register_command(&calibrate_offset_command);
  shell_register_command(&calibrate_gain_command);
  shell_register_command(&offset_command);
  shell_register_command(&gain_command);
  shell_register_command(&channel_command);
  shell_register_command(&read_register_command);
}
