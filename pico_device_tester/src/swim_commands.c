#include "swim_commands.h"
#include "read_hex_string.h"
#include <shell.h>
#include <board.h>
#include <swim.h>
#include <hardware/structs/systick.h>
#include <stdlib.h>

static int enter_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem enter_command_items[] = {
    {NULL, NULL, enter_handler}
};
static const ShellCommand enter_command = {
    enter_command_items,
    "swim_enter",
    NULL
};

static int srst_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem srst_command_items[] = {
    {NULL, NULL, srst_handler}
};
static const ShellCommand srst_command = {
    srst_command_items,
    "swim_srst",
    NULL
};

static int reset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem reset_command_items[] = {
    {NULL, NULL, reset_handler}
};
static const ShellCommand reset_command = {
    reset_command_items,
    "swim_reset",
    NULL
};

static int clock_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem clock_get_command_items[] = {
    {NULL, NULL, clock_get_handler}
};
static const ShellCommand clock_get_command = {
    clock_get_command_items,
    "swim_clock_get",
    NULL
};

static int clock_set_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem clock_set_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, clock_set_handler}
};
static const ShellCommand clock_set_command = {
    clock_set_command_items,
    "swim_clock_set",
    "swim_clock_set value rdelay"
};

static int write_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem write_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, write_handler}
};
static const ShellCommand write_command = {
    write_command_items,
    "swim_write",
    "swim_write address data"
};

static int write_csr_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem write_csr_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, write_csr_handler}
};
static const ShellCommand write_csr_command = {
    write_csr_command_items,
    "swim_write_csr",
    "swim_write_csr address data"
};

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem read_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, read_handler}
};
static const ShellCommand read_command = {
    read_command_items,
    "swim_read",
    "swim_read address num_bytes"
};

static int enter_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  unsigned int rc, clock;

  rc = swim_enter(&clock);
  pfunc("swim_enter returned %u, clock is %u\n", rc, clock);
  return 0;
}

static int srst_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("swim_srst rc=%u\n", swim_srst());
  return 0;
}

static int reset_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  swim_reset();
  return 0;
}

static int clock_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("swim base clock = %u\n", swim_get_base_clock());
  return 0;
}

static int clock_set_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int bclock = atoi(argv[0]);
  if (bclock <= 0)
  {
    pfunc("Invalid base clock\n");
    return 1;
  }
  swim_init(bclock);
  return 0;
}

static int write_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int rc;
  unsigned char data[20];
  unsigned int address;

  data[0] = data[1] = data[2] = 0;
  rc = read_hex_string(argv[0], data, 3);
  if (rc <= 0)
  {
    pfunc("Invalid address\n");
    return 1;
  }
  switch (rc)
  {
    case 1:
      address = data[0];
      break;
    case 2:
      address = (data[0] << 8) | data[1];
      break;
    default:
      address = (data[0] << 16) | (data[1] << 8) | data[2];
      break;
  }
  rc = read_hex_string(argv[1], data, sizeof data);
  if (rc <= 0)
  {
    pfunc("Invalid data\n");
    return 2;
  }
  pfunc("address %X num_bytes %d\n", address, rc);
  return (int)swim_write(address, rc, data);
}

static int write_csr_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int rc;
  unsigned char data;
  rc = read_hex_string(argv[0], &data, 1);
  if (rc <= 0)
  {
    pfunc("Invalid data\n");
    return 1;
  }
  pfunc("address %X data %X\n", SWIM_CSR, data);
  return (int)swim_write(SWIM_CSR, 1, &data);
}

static int read_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int rc, nbytes;
  unsigned char data[20];
  unsigned int address;

  data[0] = data[1] = data[2] = 0;
  rc = read_hex_string(argv[0], data, 3);
  if (rc <= 0)
  {
    pfunc("Invalid address\n");
    return 1;
  }
  switch (rc)
  {
    case 1:
      address = data[0];
      break;
    case 2:
      address = (data[0] << 8) | data[1];
      break;
    default:
      address = (data[0] << 16) | (data[1] << 8) | data[2];
      break;
  }
  nbytes = atoi(argv[1]);
  if (nbytes <= 0 || nbytes > sizeof data)
  {
    pfunc("Invalid data size");
    return 2;
  }

  pfunc("address %X num_bytes %d\n", address, nbytes);
  rc = (int)swim_read(address, nbytes, data);
  if (rc)
    return rc;
  pfunc("data: ");
  for (rc = 0; rc < nbytes; rc++)
    pfunc("%02X", data[rc]);
  pfunc("\n");
  return 0;
}

void register_swim_commands(void)
{
  swim_init(SWIM_COUNTER_MHZ);
  shell_register_command(&enter_command);
  shell_register_command(&srst_command);
  shell_register_command(&reset_command);
  shell_register_command(&clock_get_command);
  shell_register_command(&clock_set_command);
  shell_register_command(&write_command);
  shell_register_command(&write_csr_command);
  shell_register_command(&read_command);
}
