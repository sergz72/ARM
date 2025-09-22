#include "board.h"
#include "ad7793_commands.h"
#include <shell.h>
#include <ad7793.h>
#include <stdlib.h>
#include <string.h>

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init_command_items[] = {
  {NULL, NULL, init_handler}
};
static const ShellCommand init_command = {
  init_command_items,
  "ad7793_init",
  "ad7793_init",
  NULL,
  NULL
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "ad7793_read",
  "ad7793_read channel gain",
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
  "ad7793_read_register",
  "ad7793_read_register reg_no length",
  NULL,
  NULL
};

static int calibrate_offset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem calibrate_offset_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, calibrate_offset_handler}
};
static const ShellCommand calibrate_offset_command = {
  calibrate_offset_command_items,
  "ad7793_cal_offset",
  "ad7793_cal_offset channel gain internal|system",
  NULL,
  NULL
};

static int calibrate_gain_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem calibrate_gain_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, calibrate_gain_handler}
};
static const ShellCommand calibrate_gain_command = {
  calibrate_gain_command_items,
  "ad7793_cal_gain",
  "ad7793_cal_gain channel gain internal|system|value",
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
  "ad7793_gain",
  "ad7793_gain channel_no [value]",
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
  "ad7793_offset",
  "ad7793_offset channel_no [value]",
  NULL,
  NULL
};

static int io_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem io_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, io_handler}
};
static const ShellCommand io_command = {
  io_command_items,
  "ad7793_io",
  "ad7793_io iexcdir iexcen",
  NULL,
  NULL
};

static int bias_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem bias_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, bias_handler}
};
static const ShellCommand bias_command = {
  bias_command_items,
  "ad7793_bias",
  "ad7793_bias value",
  NULL,
  NULL
};

static const ad7793_configuration ad7793_config =
{
  .channel = AD7793_CHANNEL_AIN1N_AIN1N,
  .boost = 1,
  .buffer_enable = 1,
  .burnout_enable = 0,
  .gain = AD7793_GAIN_1,
  .internal_reference = 1,
  .unipolar = 1,
  .vbias = AD7793_VBIAS_OFF
};

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  set_cpol1cpha1();
  ad7793_set_mode_and_update_rate(0, AD7793_MODE_IDLE, AD7793_UPDATE_RATE_16_80);
  ad7793_set_configuration(0, &ad7793_config);
  return 0;
}

static int read_channel(char *arg)
{
  int channel = atoi(arg);
  if (channel < 0 || channel > 7 || channel == 4 || channel == 5)
    return -1;
  return channel;
}

static int read_voltage_func(printf_func pfunc, unsigned char channel, unsigned char gain)
{
  ad7793_read_voltage_configuration configuration =
  {
    .channel = 0,
    .channel_no = channel,
    .gain = gain,
    .vref = AD7793_VREF,
    .input_divider_coefficient = AD7793_INPUT_COEFFICIENT
  };
  ad7793_data result;
  int rc = ad7793_read_voltage(&configuration, &result, AD7793_TIMEOUT);
  if (rc)
    return rc;
  pfunc("code = %08X, result = %f V\r\n", result.value, result.voltage);
  return 0;
}

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = read_channel(argv[0]);
  if (channel < 0)
    return 1;
  int gain = atoi(argv[1]);
  if (gain < 0 || gain > 7)
    return 2;
  return read_voltage_func(pfunc, channel, gain);
}

static int read_register_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int reg_no = atoi(argv[0]);
  if (reg_no < 0 || reg_no > 7)
    return 1;
  int length = atoi(argv[1]);
  if (length < 1 || length > 3)
    return 2;
  unsigned char reg_data[3];
  ad7793_read_register(0, reg_no, reg_data, length);
  for (int i = 0; i < length; i++)
    pfunc("%02X ", reg_data[i]);
  pfunc("\r\n");
  return 0;
}

static int calibrate_offset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = read_channel(argv[0]);
  if (channel < 0)
    return 1;
  int gain = atoi(argv[1]);
  if (gain < 0 || gain > 7)
    return 2;
  int rc;
  if (!strcmp(argv[2], "internal"))
    rc = ad7793_calibrate_offset_internal(0, channel, gain, AD7793_TIMEOUT);
  else if (!strcmp(argv[2], "system"))
    rc = ad7793_calibrate_offset_system(0, channel, gain, AD7793_TIMEOUT);
  else
    return 1;
  if (rc)
  {
    pfunc("calibrate offset returned error %d\r\n", rc);
    return rc;
  }
  return read_voltage_func(pfunc, channel, gain);
}

static int calibrate_gain_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = read_channel(argv[0]);
  if (channel < 0)
    return 1;
  int gain = atoi(argv[1]);
  if (gain < 0 || gain > 7)
    return 2;
  int rc;
  if (!strcmp(argv[1], "internal"))
    rc = ad7793_calibrate_gain_internal(0, channel, gain, AD7793_TIMEOUT);
  else if (!strcmp(argv[1], "system"))
    rc = ad7793_calibrate_gain_system(0, channel, gain, AD7793_TIMEOUT);
  else
    return 1;
  if (rc)
  {
    pfunc("calibrate gain returned error %d\r\n", rc);
    return rc;
  }
  return read_voltage_func(pfunc, channel, gain);
}

static int gain_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = read_channel(argv[0]);
  if (channel < 0)
    return 1;
  if (argc == 1)
  {
    int gain = ad7793_get_gain(0, channel);
    pfunc("gain = %08X\r\n", gain);
  }
  else
  {
    int gain = strtoul(argv[1], NULL, 16);
    ad7793_set_gain(0, channel, gain);
  }
  return 0;
}

static int offset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = read_channel(argv[0]);
  if (channel < 0)
    return 1;
  if (argc == 1)
  {
    int offset = ad7793_get_offset(0, channel);
    pfunc("offset = %08X\r\n", offset);
  }
  else
  {
    int offset = strtoul(argv[1], NULL, 16);
    ad7793_set_offset(0, channel, offset);
  }
  return 0;
}

static int io_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int iexcdir = atoi(argv[0]);
  if (iexcdir < 0 || iexcdir > 3)
    return 1;
  int iexcen = atoi(argv[1]);
  if (iexcen < 0 || iexcen > 3)
    return 2;
  ad7793_set_io(0, iexcdir, iexcen);
  return 0;
}

static int bias_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int bias = atoi(argv[0]);
  if (bias < 0 || bias > 2)
    return 1;
  ad7793_set_bias(0, bias);
  return 0;
}

void register_ad7793_commands(void)
{
  shell_register_command(&init_command);
  shell_register_command(&read_command);
  shell_register_command(&calibrate_offset_command);
  shell_register_command(&calibrate_gain_command);
  shell_register_command(&offset_command);
  shell_register_command(&gain_command);
  shell_register_command(&read_register_command);
  shell_register_command(&io_command);
  shell_register_command(&bias_command);
}
