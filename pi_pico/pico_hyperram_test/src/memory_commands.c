#include "memory_commands.h"
#include <read_hex_string.h>
#include <shell.h>
#include <stdlib.h>
#include <string.h>
#include <chacha.h>
#include "hyperram.h"

#define ADDRESS_LENGTH 3

static int write_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem write_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, write_handler}
};
static const ShellCommand write_command = {
    write_command_items,
    "write",
    "write address data"
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
  read_command_items,
  "read",
  "read address num_bytes"
};

static int write_random_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem write_random_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, write_random_handler}
};
static const ShellCommand write_random_command = {
  write_random_command_items,
  "write_random",
  "write_random address num_bytes"
};

static int check_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem check_command_items[] = {
  {NULL, param_handler, NULL},
  {NULL, param_handler, NULL},
  {NULL, NULL, check_handler}
};
static const ShellCommand check_command = {
  check_command_items,
  "check",
  "check address num_bytes"
};

static ChaCha rng;

static int parse_address(printf_func pfunc, char *arg, unsigned int *address)
{
  unsigned char data[ADDRESS_LENGTH];
  int i;

  int rc = read_hex_string(arg, data, ADDRESS_LENGTH);
  if (rc <= 0)
  {
    pfunc("Invalid address\r\n");
    return 1;
  }

  unsigned int a = 0;
  for (i = 0; i < ADDRESS_LENGTH; i++)
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
  static unsigned char data[2000];
  unsigned int address;

  if (parse_address(pfunc, argv[0], &address))
    return 1;

  nbytes = atoi(argv[1]);
  if (nbytes <= 0 || nbytes > sizeof data)
  {
    pfunc("Invalid data size\r\n");
    return 2;
  }

  hyperram_read(address, data, nbytes);
  pfunc("data: \r\n");
  for (i = 0; i < 16; i++)
    pfunc("%02X ", i);
  for (i = 0; i < nbytes; i++)
  {
    if (!(i & 0x0F))
      pfunc("\r\n");
    pfunc("%02X ", data[i]);
  }
  pfunc("\r\n");

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

  if (rc > sizeof data)
  {
    pfunc("Invalid data size\r\n");
    return 3;
  }

  pfunc("num_bytes %d\n", rc);
  hyperram_write(address, data, rc);
  return 0;
}

static unsigned char next_byte(void)
{
  return chacha_u8(&rng);
}

static int write_random_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int nbytes;
  unsigned int address;

  if (parse_address(pfunc, argv[0], &address))
    return 1;

  nbytes = atoi(argv[1]);
  if (nbytes <= 0)
  {
    pfunc("Invalid data size\r\n");
    return 2;
  }

  chacha20_zero(&rng, 0);

  pfunc("num_bytes %d\n", nbytes);
  hyperram_write_callback(address, next_byte, nbytes);
  return 0;
}

static int set_byte(unsigned char byte)
{
  return chacha_u8(&rng) != byte;
}

static int check_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int nbytes;
  unsigned int address;

  if (parse_address(pfunc, argv[0], &address))
    return 1;

  nbytes = atoi(argv[1]);
  if (nbytes <= 0)
  {
    pfunc("Invalid data size\r\n");
    return 2;
  }

  chacha20_zero(&rng, 0);

  pfunc("num_bytes %d\n", nbytes);
  return hyperram_read_callback(address, set_byte, nbytes);
}

void register_memory_commands(void)
{
  shell_register_command(&read_command);
  shell_register_command(&write_command);
  shell_register_command(&write_random_command);
  shell_register_command(&check_command);
}
