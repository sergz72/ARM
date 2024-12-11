#include "spi_memory_commands.h"
#include <read_hex_string.h>
#include <shell.h>
#include <stdlib.h>
#include "board.h"
#include <spi_memory.h>

typedef struct
{
  int address_length;
  unsigned int (*read_id)(int channel);
  void (*wren)(int channel);
  void (*read)(int channel, unsigned int address, unsigned char *buffer, int count);
  void (*write)(int channel, unsigned int address, unsigned char *buffer, int count);
  int (*read_callback)(int channel, unsigned int address, int (*set_byte)(unsigned char c), int count);
  void (*write_callback)(int channel, unsigned int address, unsigned char (*next_byte)(void), int count);
} spi_memory;

static const spi_memory psram = {
  .address_length = 3,
  .read_id = psram_read_id,
  .wren = NULL,
  .read = psram_fast_read,
  .write = psram_write,
  .read_callback = psram_fast_read_cb,
  .write_callback = psram_write_cb,
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

static int type_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return 0;
}

static int read_id_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned int id = device->read_id(0);
  pfunc("Device id: %u\r\n", id);
  return 0;
}

static int parse_address(printf_func pfunc, char *arg, unsigned int *address)
{
  unsigned char data[device->address_length];
  int i;

  int rc = read_hex_string(arg, data, device->address_length);
  if (rc <= 0)
  {
    pfunc("Invalid address\n");
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
  if (nbytes <= 0 || nbytes > sizeof data)
  {
    pfunc("Invalid data size");
    return 2;
  }

  device->read(0, address, data, nbytes);
  pfunc("data: ");
  for (i = 0; i < nbytes; i++)
    pfunc("%02X", data[i]);
  pfunc("\n");

  return 0;
}

static int write_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int rc, nbytes;
  unsigned char data[20];
  unsigned int address;

  if (parse_address(pfunc, argv[0], &address))
    return 1;

  rc = read_hex_string(argv[0], data, sizeof data);
  if (rc <= 0)
  {
    pfunc("Invalid data\n");
    return 2;
  }

  nbytes = atoi(argv[1]);
  if (nbytes <= 0 || nbytes > sizeof data)
  {
    pfunc("Invalid data size");
    return 2;
  }

  pfunc("num_bytes %d\n", rc);
  device->write(0, address, data, nbytes);
  pfunc("data: ");
  for (rc = 0; rc < nbytes; rc++)
    pfunc("%02X", data[rc]);
  pfunc("\n");

  return 0;
}

void register_spi_memory_commands(void)
{
  shell_register_command(&type_command);
  shell_register_command(&read_id_command);
  shell_register_command(&read_command);
  shell_register_command(&write_command);
}
