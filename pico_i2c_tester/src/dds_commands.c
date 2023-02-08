#include "board.h"
#include "dds_commands.h"
#include <shell.h>
#include <stddef.h>
#include <stdlib.h>
#include <generic_dds.h>
#include <hardware/i2c.h>
#include <string.h>

typedef struct __attribute__((__packed__)) {
  unsigned char command;
  unsigned char channel;
  unsigned char parameter;
} cmd3;

typedef struct __attribute__((__packed__)) {
  unsigned char command;
  unsigned char channel;
  unsigned short parameter;
} cmd4;

typedef struct __attribute__((__packed__)) {
  unsigned char  command;
  unsigned char  channel;
  unsigned int   freq;
  unsigned short div;
} cmd8;

static int deviceId, max_div;
static dds_config config;

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
    read_command_items,
    "dds_read",
    "dds_read deviceId"
};

static int freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem freq_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, freq_handler}
};
static const ShellCommand freq_command = {
    freq_command_items,
    "dds_freq",
    "dds_freq channel frequency divider"
};

static int mode_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem mode_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, mode_handler}
};
static const ShellCommand mode_command = {
    mode_command_items,
    "dds_mode",
    "dds_mode channel sin|tri|sq"
};

static int att_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem att_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, att_handler}
};
static const ShellCommand att_command = {
    att_command_items,
    "dds_att",
    "dds_att channel value"
};

static int enable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem enable_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, enable_handler}
};
static const ShellCommand enable_command = {
    enable_command_items,
    "dds_enable",
    "dds_enable channel"
};

static int disable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem disable_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, disable_handler}
};
static const ShellCommand disable_command = {
    disable_command_items,
    "dds_disable",
    "dds_disable channel"
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  deviceId = atoi(argv[0]);
  if (deviceId <= 0 || deviceId > 0x7F)
  {
    pfunc("Invalid address\n");
    return 1;
  }
  int rc = i2c_read_timeout_us(i2c_default, deviceId, (unsigned char*)&config, sizeof(dds_config), false, I2C_TIMEOUT);
  if (rc == PICO_ERROR_GENERIC)
  {
    pfunc("I2C read error\n");
    return 2;
  }
  if (rc == PICO_ERROR_TIMEOUT)
  {
    pfunc("I2C read timeout\n");
    return 3;
  }

  max_div = 1 << ((1 << config.out_square_divider_bits) - 1);

  pfunc("min frequency: %d\n", config.min_frequency);
  pfunc("max frequency: %d\n", config.max_frequency);
  pfunc("channels: %d\n", config.channels);
  pfunc("MCLK: %d MHz\n", config.mclk_MHz);
  pfunc("accumulator bits: %d\n", config.accumulator_bits);
  pfunc("max attenuator value: %d\n", config.max_attenuator_value);
  pfunc("max vout: %d mV\n", config.max_vout_mV);
  pfunc("square divider bits: %d\n", config.out_square_divider_bits);
  pfunc("supported modes: %d\n", config.supported_modes);

  return 0;
}

static int i2c_write(printf_func pfunc, int retv, const unsigned char *src, size_t len)
{
  int rc = i2c_write_timeout_us(i2c_default, deviceId, src, len, false, I2C_TIMEOUT);
  if (rc == PICO_ERROR_GENERIC)
  {
    pfunc("I2C write error\n");
    return retv;
  }
  if (rc == PICO_ERROR_TIMEOUT)
  {
    pfunc("I2C write timeout\n");
    return retv + 1;
  }

  return 0;
}

static void print_command(printf_func pfunc, unsigned char *c, int l)
{
  switch (l)
  {
    case 3:
      pfunc("data to send: %02X %02X %02X\n", c[0], c[1], c[2]);
      break;
    case 4:
      pfunc("data to send: %02X %02X %02X %02X\n", c[0], c[1], c[2], c[3]);
      break;
    case 8:
      pfunc("data to send: %02X %02X %02X %02X %02X %02X %02X %02X\n", c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7]);
      break;
  }
}

static unsigned int calculateFrequencyCode(unsigned int frequency)
{
  return ((unsigned long long int)frequency << config.accumulator_bits) / (config.mclk_MHz * 1000000);
}

static int freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel, freq, div;
  cmd8 c;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= config.channels)
  {
    pfunc("Invalid channel\n");
    return 1;
  }
  freq = atoi(argv[1]);
  if (freq < config.min_frequency || freq > config.max_frequency)
  {
    pfunc("Invalid frequency\n");
    return 2;
  }
  div = atoi(argv[2]);
  if (div < 1 || div > max_div)
  {
    pfunc("Invalid divider\n");
    return 2;
  }

  if (config.accumulator_bits)
  {
    c.command = DDS_COMMAND_SET_FREQUENCY_CODE;
    c.freq = calculateFrequencyCode(freq);
  }
  else
  {
    c.command = DDS_COMMAND_SET_FREQUENCY;
    c.freq = freq;
  }
  c.channel = channel;
  c.div = div;

  print_command(pfunc, (unsigned char*)&c, 8);

  return i2c_write(pfunc, 3, (unsigned char*)&c, 8);
}

static int mode_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel;
  cmd3 c;
  unsigned char mode;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= config.channels)
  {
    pfunc("Invalid channel\n");
    return 1;
  }

  if (!strcmp(argv[1], "sin"))
    mode = DDS_MODE_SINE;
  else if (!strcmp(argv[1], "tri"))
    mode = DDS_MODE_TRIANGLE;
  else if (!strcmp(argv[1], "sq"))
    mode = DDS_MODE_SQUARE;
  else
  {
    pfunc("Invalid mode\n");
    return 1;
  }
  if (!(config.supported_modes & (1<< mode)))
  {
    pfunc("Mode is not supported\n");
    return 2;
  }

  c.command = DDS_COMMAND_SET_MODE;
  c.channel = channel;
  c.parameter = mode;

  print_command(pfunc, (unsigned char*)&c, 3);

  return i2c_write(pfunc, 3, (unsigned char*)&c, 3);
}

static int enable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel;
  cmd3 c;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= config.channels)
  {
    pfunc("Invalid channel\n");
    return 1;
  }

  c.command = DDS_COMMAND_ENABLE_OUTPUT;
  c.channel = channel;
  c.parameter = 1;

  print_command(pfunc, (unsigned char*)&c, 3);

  return i2c_write(pfunc, 2, (unsigned char*)&c, 3);
}

static int disable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel;
  cmd3 c;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= config.channels)
  {
    pfunc("Invalid channel\n");
    return 1;
  }

  c.command = DDS_COMMAND_ENABLE_OUTPUT;
  c.channel = channel;
  c.parameter = 0;

  print_command(pfunc, (unsigned char*)&c, 3);

  return i2c_write(pfunc, 2, (unsigned char*)&c, 3);
}

static int att_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int channel, att;
  cmd4 c;

  channel = atoi(argv[0]);
  if (channel < 0 || channel >= config.channels)
  {
    pfunc("Invalid channel\n");
    return 1;
  }
  att = atoi(argv[1]);
  if (att < 0 || att > config.max_attenuator_value)
  {
    pfunc("Invalid value\n");
    return 2;
  }

  c.command = DDS_COMMAND_SET_ATTENUATOR;
  c.channel = channel;
  c.parameter = att;

  print_command(pfunc, (unsigned char*)&c, 4);

  return i2c_write(pfunc, 2, (unsigned char*)&c, 4);
}

void register_dds_commands(void)
{
  shell_register_command(&read_command);
  shell_register_command(&freq_command);
  shell_register_command(&mode_command);
  shell_register_command(&enable_command);
  shell_register_command(&disable_command);
  shell_register_command(&att_command);
}
