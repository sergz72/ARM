#include "board.h"
#include "ad7705_commands.h"
#include <shell.h>
#include <stdlib.h>
#include <ad7705.h>
#include <string.h>

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem init_command_items[] = {
  {NULL, NULL, init_handler}
};
static const ShellCommand init_command = {
  init_command_items,
  "ad7705_init",
  "ad7705_init",
  NULL,
  NULL
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "ad7705_read",
  "ad7705_read channel",
  NULL,
  NULL
};

static int calibrate_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem calibrate_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, calibrate_handler}
};
static const ShellCommand calibrate_command = {
  calibrate_command_items,
  "ad7705_cal",
  "ad7705_cal channel self|zero|full",
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
  "ad7705_gain",
  "ad7705_gain channel_no [value]",
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
  "ad7705_offset",
  "ad7705_offset channel_no [value]",
  NULL,
  NULL
};

static int pga_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem pga_command_items[] = {
  {NULL, param_handler, pga_handler},
  {NULL, NULL, pga_handler}
};
static const ShellCommand pga_command = {
  pga_command_items,
  "ad7705_pga",
  "ad7705_pga [value]",
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
  "ad7705_read_register",
  "ad7705_read_register read_register reg_no length",
  NULL,
  NULL
};

static int start_conversion_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem start_conversion_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, start_conversion_handler}
};
static const ShellCommand start_conversion_command = {
  start_conversion_command_items,
  "ad7705_start_conversion",
  "ad7705_start_conversion channel",
  NULL,
  NULL
};

static int stop_conversion_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem stop_conversion_command_items[] = {
  {NULL, NULL, stop_conversion_handler}
};
static const ShellCommand stop_conversion_command = {
  stop_conversion_command_items,
  "ad7705_stop_conversion",
  "ad7705_stop_conversion",
  NULL,
  NULL
};

static const ad7705_configuration ad7705_config =
{
  .unipolar = 0,
  .buf = 1,
  .pga_gain = AD7705_PGA_GAIN_1,
  .filter = AD7705_FILTER_50Hz,
  .clkdis = 0,
  .clkdiv = 1,
  .clk = 1,
  .fsync = 1
};

static int init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  set_cpol1cpha1();
  ad7705_setup_configuration(0, &ad7705_config);
  return 0;
}

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = atoi(argv[0]);
  if (channel < 0 || channel > 3)
    return 1;
  ad7705_data adata;
  int rc = ad7705_read_voltage(0, channel, AD7705_INPUT_COEFFICIENT, AD7705_VREF, &adata, AD7705_TIMEOUT);
  if (rc)
    return rc;
  pfunc("code = %08X, result = %f\r\n", adata.value, adata.voltage);
  return 0;
}

static int calibrate_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = atoi(argv[0]);
  if (channel < 0 || channel > 3)
    return 1;
  unsigned char mode;
  if (!strcmp(argv[1], "self"))
    mode = AD7705_MODE_SELF_CALIBRATION;
  else if (!strcmp(argv[1], "zero"))
    mode = AD7705_MODE_ZERO_SCALE_CALIBRATION;
  else if (!strcmp(argv[1], "full"))
    mode = AD7705_MODE_FULL_SCALE_CALIBRATION;
  else
    return 1;
  return ad7705_calibration(0, channel, mode, AD7705_TIMEOUT);
}

static int gain_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = atoi(argv[0]);
  if (channel < 0 || channel > 3)
    return 1;
  if (argc == 1)
  {
    int gain = ad7705_get_gain(0, channel);
    pfunc("gain = %08X\r\n", gain);
  }
  else
  {
    int gain = strtoul(argv[1], NULL, 16);
    ad7705_set_gain(0, channel, gain);
  }
  return 0;
}

static int offset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = atoi(argv[0]);
  if (channel < 0 || channel > 3)
    return 1;
  if (argc == 1)
  {
    int gain = ad7705_get_offset(0, channel);
    pfunc("offset = %08X\r\n", gain);
  }
  else
  {
    int gain = strtoul(argv[1], NULL, 16);
    ad7705_set_offset(0, channel, gain);
  }
  return 0;
}

static int pga_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  if (argc == 0)
  {
    int pga = ad7705_get_pga(0);
    pfunc("pga = %d\r\n", pga);
  }
  else
  {
    int pga = atoi(argv[0]);
    ad7705_set_pga(0, pga);
  }
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
  ad7705_read_register(0, reg_no, reg_data, length);
  for (int i = 0; i < length; i++)
    pfunc("%02X ", reg_data[i]);
  pfunc("\r\n");
  return 0;
}

static int start_conversion_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel = atoi(argv[0]);
  if (channel < 0 || channel > 3)
    return 1;
  ad7705_start_conversion(0, channel);
  return 0;
}

static int stop_conversion_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  ad7705_stop_conversion(0);
  return 0;
}

void register_ad7705_commands(void)
{
  shell_register_command(&init_command);
  shell_register_command(&read_command);
  shell_register_command(&calibrate_command);
  shell_register_command(&offset_command);
  shell_register_command(&gain_command);
  shell_register_command(&read_register_command);
  shell_register_command(&pga_command);
  shell_register_command(&start_conversion_command);
  shell_register_command(&stop_conversion_command);
}