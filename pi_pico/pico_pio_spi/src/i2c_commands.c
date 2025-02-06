#include "board.h"
#include "i2c_commands.h"
#include <shell.h>
#include <stddef.h>
#include <stdlib.h>
#include <hardware/i2c.h>
#include <read_hex_string.h>
#include <string.h>

static int i2cwrite_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem i2cwrite_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, i2cwrite_handler}
};
static const ShellCommand i2cwrite_command = {
    i2cwrite_command_items,
    "i2c_write",
    "i2c_write address data"
};

static int i2cread_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem i2cread_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, i2cread_handler}
};
static const ShellCommand i2cread_command = {
    i2cread_command_items,
    "i2c_read",
    "i2c_read address send_data|NONE num_bytes_to_read"
};

static int i2cbaud_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem i2cbaud_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, i2cbaud_handler}
};
static const ShellCommand i2cbaud_command = {
    i2cbaud_command_items,
    "i2c_baud",
    "i2c_baud speed_in_khz"
};

static int i2cbaud_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int baud = atoi(argv[0]);
  if (baud <= 0 || baud > 400)
  {
    pfunc("Invalid baud rate\n");
    return 1;
  }
  baud = (int)i2c_set_baudrate(I2C_INST, baud * 1000);
  pfunc("I2C baud rate has been set to %dHz\n", baud);
  return 0;
}

static int i2cwrite_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int i, rc;
  unsigned char data[20], address;

  rc = read_hex_string(argv[0], data, 1);
  if (rc <= 0 || data[0] > 0x7F)
  {
    pfunc("Invalid address\n");
    return 1;
  }
  address = data[0];
  rc = read_hex_string(argv[1], data, sizeof data);
  if (rc <= 0)
  {
    pfunc("Invalid data\n");
    return 2;
  }
  pfunc("i2c write %d bytes to address %02X\ndata: ", rc, address);
  for (i = 0; i < rc; i++)
    pfunc("%02X", data[i]);
  pfunc("\n");
  rc = i2c_write_timeout_us(I2C_INST, address, data, rc, false, I2C_TIMEOUT);
  if (rc == PICO_ERROR_GENERIC)
  {
    pfunc("I2C write error\n");
    return 3;
  }
  if (rc == PICO_ERROR_TIMEOUT)
  {
    pfunc("I2C write timeout\n");
    return 4;
  }
  return 0;
}

static int i2cread_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int i, rc, num_bytes;
  unsigned char data[50], address;

  rc = read_hex_string(argv[0], data, 1);
  if (rc <= 0 || data[0] > 0x7F)
  {
    pfunc("Invalid address\n");
    return 1;
  }
  address = data[0];
  if (strcmp(argv[1], "NONE"))
  {
    rc = read_hex_string(argv[1], data, sizeof data);
    if (rc <= 0)
    {
      pfunc("Invalid send_data\n");
      return 2;
    }
  }
  else
    rc = 0;
  num_bytes = atoi(argv[2]);
  if (num_bytes <= 0 || num_bytes > sizeof data)
  {
    pfunc("Invalid num_bytes_to_read\n");
    return 3;
  }
  if (rc)
  {
    pfunc("i2c write %d bytes to address %02X\ndata: ", rc, address);
    for (i = 0; i < rc; i++)
      pfunc("%02X", data[i]);
    pfunc("\n");
    rc = i2c_write_timeout_us(I2C_INST, address, data, rc, true, I2C_TIMEOUT);
    if (rc == PICO_ERROR_GENERIC) // true to keep master control of bus
    {
      pfunc("I2C write error\n");
      return 4;
    }
    if (rc == PICO_ERROR_TIMEOUT)
    {
      pfunc("I2C write timeout\n");
      return 5;
    }
  }
  pfunc("i2c read %d bytes\n", num_bytes);
  rc = i2c_read_timeout_us(I2C_INST, address, data, num_bytes, false, I2C_TIMEOUT);
  if (rc == PICO_ERROR_GENERIC)
  {
    pfunc("I2C read error\n");
    return 6;
  }
  if (rc == PICO_ERROR_TIMEOUT)
  {
    pfunc("I2C read timeout\n");
    return 7;
  }
  pfunc("data: ");
  for (i = 0; i < rc; i++)
    pfunc("%02X", data[i]);
  pfunc("\n");
  return 0;
}

void register_i2c_commands(void)
{
  shell_register_command(&i2cwrite_command);
  shell_register_command(&i2cread_command);
  shell_register_command(&i2cbaud_command);
}
