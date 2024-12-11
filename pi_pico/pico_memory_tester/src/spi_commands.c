#include "spi_commands.h"
#include <read_hex_string.h>
#include <shell.h>
#include <stdlib.h>

#include "board.h"

static int clock_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem clock_get_command_items[] = {
    {NULL, NULL, clock_get_handler}
};
static const ShellCommand clock_get_command = {
    clock_get_command_items,
    "spi_clock_get",
    NULL
};

static int clock_set_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem clock_set_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, clock_set_handler}
};
static const ShellCommand clock_set_command = {
    clock_set_command_items,
    "spi_clock_set",
    "spi_clock_set value"
};

static int trfr_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem trfr_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, trfr_handler}
};
static const ShellCommand trfr_command = {
    trfr_command_items,
    "spi_trfr",
    "spi_trfr write_data read_count"
};

static int clock_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  pfunc("spi clock = %u\n", spi_get_clock(0));
  return 0;
}

static int clock_set_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int clock = atoi(argv[0]);
  if (clock <= 0)
  {
    pfunc("Invalid clock value\n");
    return 1;
  }
  spi_set_clock(0, clock);
  pfunc("SPI clock set to %u", spi_get_clock(0));
  return 0;
}

static int trfr_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int rc, nbytes;
  unsigned char data[20];

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
  spi_trfr(0, rc, data, nbytes, data);
  pfunc("data: ");
  for (rc = 0; rc < nbytes; rc++)
    pfunc("%02X", data[rc]);
  pfunc("\n");

  return 0;
}

void register_spi_commands(void)
{
  shell_register_command(&clock_get_command);
  shell_register_command(&clock_set_command);
  shell_register_command(&trfr_command);
}
