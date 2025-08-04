#include "spi_commands.h"
#include <read_hex_string.h>
#include <shell.h>
#include <stdlib.h>
#include <spi_memory.h>

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
  spi_trfr(0, rc, data, 0, nbytes, data, 1);
  pfunc("data: ");
  for (rc = 0; rc < nbytes; rc++)
    pfunc("%02X", data[rc]);
  pfunc("\n");

  return 0;
}

void register_spi_commands(void)
{
  shell_register_command(&trfr_command);
}
