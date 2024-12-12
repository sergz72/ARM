#include "spi_memory_commands.h"
#include <read_hex_string.h>
#include <shell.h>
#include <stdlib.h>
#include <spi_memory.h>
#include <string.h>
#include <93cXX_16.h>
#include <chacha.h>
#include <limits.h>

int _93cxx16_read(int channel, unsigned int address, unsigned char *buffer, int count);
int _93cxx16_write(int channel, unsigned int address, unsigned char *buffer, int count);
int _93cxx16_write_enable(int channel);

typedef struct
{
  int address_length;
  int max_data_length;
  unsigned int (*read_id)(int channel);
  int (*wren)(int channel);
  int (*read)(int channel, unsigned int address, unsigned char *buffer, int count);
  int (*write)(int channel, unsigned int address, unsigned char *buffer, int count);
  int (*read_callback)(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count);
  int (*write_callback)(int channel, unsigned int address, unsigned char (*next_byte)(void), int count);
} spi_memory;

static const spi_memory psram = {
  .address_length = 3,
  .max_data_length = INT_MAX,
  .read_id = psram_read_id,
  .wren = NULL,
  .read = psram_fast_read,
  .write = psram_write,
  .read_callback = psram_fast_read_cb,
  .write_callback = psram_write_cb,
};

static const spi_memory _93cxx16 = {
  .address_length = 1,
  .max_data_length = 2,
  .read_id = NULL,
  .wren = _93cxx16_write_enable,
  .read = _93cxx16_read,
  .write = _93cxx16_write,
  .read_callback = NULL,
  .write_callback = NULL,
};

static const spi_memory *device = &psram;

static int type_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem type_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, NULL, type_handler}
};
static const ShellCommand type_command = {
  type_command_items,
  "spi_mem_type",
  "spi_mem_type memory_type"
};

static int read_id_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_id_command_items[] = {
  {NULL, NULL, read_id_handler}
};
static const ShellCommand read_id_command = {
  read_id_command_items,
  "spi_mem_read_id",
  "spi_mem_read_id"
};

static int wren_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem wren_command_items[] = {
  {NULL, NULL, wren_handler}
};
static const ShellCommand wren_command = {
  wren_command_items,
  "spi_mem_wren",
  "spi_mem_wren"
};

static int write_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem write_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, write_handler}
};
static const ShellCommand write_command = {
    write_command_items,
    "spi_mem_write",
    "spi_mem_write address data"
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "spi_mem_read",
  "spi_mem_read address num_bytes"
};

static int write_random_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem write_random_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, write_random_handler}
};
static const ShellCommand write_random_command = {
  write_random_command_items,
  "spi_mem_write_random",
  "spi_mem_write_random address num_bytes"
};

static int check_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem check_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, check_handler}
};
static const ShellCommand check_command = {
  check_command_items,
  "spi_mem_check",
  "spi_mem_check address num_bytes"
};

static ChaCha rng;

int _93cxx16_read(int channel, unsigned int address, unsigned char *buffer, int count)
{
  unsigned short data;
  int rc = _93CXX_16_read(channel, address, &data, 0xFFFFFF);
  buffer[0] = (unsigned char)data;
  buffer[1] = (unsigned char)(data >> 8);
  return rc;
}

int _93cxx16_write(int channel, unsigned int address, unsigned char *buffer, int count)
{
  unsigned int data = buffer[0] | (buffer[1] << 8);
  return _93CXX_16_write(channel, address, data, 0xFFFFFF);
}

int _93cxx16_write_enable(int channel)
{
  return _93CXX_16_write_enable(channel, 0xFFFFFF);
}

static int type_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  if (!strcmp(argv[0], "psram"))
    device = &psram;
  else if (!strcmp(argv[0], "93cxx16"))
    device = &_93cxx16;
  else if (!strcmp(argv[0], "help"))
    pfunc("psram|93cxx16\r\n");
  else
  {
    pfunc("unknown device type.\r\n");
    return 1;
  }
  return 0;
}

static int check_supported(printf_func pfunc, void *ptr)
{
  if (ptr == NULL)
  {
    pfunc("not supported.\r\n");
    return 0;
  }
  return 1;
}

static int read_id_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  if (!check_supported(pfunc, device->read_id))
    return 1;
  unsigned int id = device->read_id(0);
  pfunc("Device id: %x\r\n", id);
  return 0;
}

static int wren_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  if (!check_supported(pfunc, device->wren))
    return 1;
  return device->wren(0);
}

static int parse_address(printf_func pfunc, char *arg, unsigned int *address)
{
  unsigned char data[device->address_length];
  int i;

  int rc = read_hex_string(arg, data, device->address_length);
  if (rc <= 0)
  {
    pfunc("Invalid address\r\n");
    return 1;
  }

  unsigned int a = 0;
  for (i = 0; i < device->address_length; i++)
  {
    a <<= 8;
    a |= data[i];
  }
  *address = a;
  return 0;
}

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int i, nbytes;
  unsigned char data[20];
  unsigned int address;

  if (parse_address(pfunc, argv[0], &address))
    return 1;

  nbytes = atoi(argv[1]);
  if (nbytes <= 0 || nbytes > sizeof data || nbytes > device->max_data_length)
  {
    pfunc("Invalid data size\r\n");
    return 2;
  }

  i = device->read(0, address, data, nbytes);
  if (i)
    return i;
  pfunc("data: ");
  for (i = 0; i < nbytes; i++)
    pfunc("%02X", data[i]);
  pfunc("\n");

  return 0;
}

static int write_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int rc;
  unsigned char data[20];
  unsigned int address;

  if (parse_address(pfunc, argv[0], &address))
    return 1;

  rc = read_hex_string(argv[1], data, sizeof data);
  if (rc <= 0)
  {
    pfunc("Invalid data\r\n");
    return 2;
  }

  if (rc > sizeof data || rc > device->max_data_length)
  {
    pfunc("Invalid data size\r\n");
    return 3;
  }

  pfunc("num_bytes %d\n", rc);
  return device->write(0, address, data, rc);
}

static unsigned char next_byte(void)
{
  return chacha_u8(&rng);
}

static int write_random_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int nbytes;
  unsigned int address;

  if (!check_supported(pfunc, device->write_callback))
    return 1;

  if (parse_address(pfunc, argv[0], &address))
    return 1;

  nbytes = atoi(argv[1]);
  if (nbytes <= 0 || nbytes > device->max_data_length)
  {
    pfunc("Invalid data size\r\n");
    return 2;
  }

  chacha20_zero(&rng, 0);

  pfunc("num_bytes %d\n", nbytes);
  return device->write_callback(0, address, next_byte, nbytes);
}

static int set_byte(unsigned char byte)
{
  return chacha_u8(&rng) != byte;
}

static int check_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int nbytes;
  unsigned int address;

  if (!check_supported(pfunc, device->read_callback))
    return 1;

  if (parse_address(pfunc, argv[0], &address))
    return 1;

  nbytes = atoi(argv[1]);
  if (nbytes <= 0 || nbytes > device->max_data_length)
  {
    pfunc("Invalid data size\r\n");
    return 2;
  }

  chacha20_zero(&rng, 0);

  pfunc("num_bytes %d\n", nbytes);
  return device->read_callback(0, address, set_byte, nbytes);
}

void register_spi_memory_commands(void)
{
  shell_register_command(&type_command);
  shell_register_command(&read_id_command);
  shell_register_command(&wren_command);
  shell_register_command(&read_command);
  shell_register_command(&write_command);
  shell_register_command(&write_random_command);
  shell_register_command(&check_command);
}
