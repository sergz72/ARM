#include "board.h"
#include "i2c_commands.h"
#include <shell.h>
#include <stdlib.h>
#include <string.h>
#include <read_hex_string.h>
#include "hardware/i2c.h"

static int scan_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem scan_command_items[] = {
  {nullptr, nullptr, scan_handler}
};
static const ShellCommand scan_command = {
  scan_command_items,
  "i2c_scan",
  "i2c_scan",
  nullptr,
  nullptr
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  {nullptr, param_handler, nullptr},
  {nullptr, param_handler, nullptr},
  {nullptr, param_handler, nullptr},
  {nullptr, nullptr, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "i2c_read",
  "i2c_read address send_data|NONE num_bytes_to_read"
};

static int scan_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
  pfunc("00:   ");

  uint8_t rxData;
  for (uint8_t i = 1; i <= 0x7F; i++)
  {
    const int rc = i2c_read_blocking(I2C_INST, i, &rxData, 1, false);
    if (i % 16 == 0)
      pfunc("\n%.2x:", i);
    if (rc >= 0)
      pfunc(" %.2x", i);
    else
      pfunc(" --");
    sleep_ms(1);
  }

  return 0;
}

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
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
  shell_register_command(&scan_command);
  shell_register_command(&read_command);
}
