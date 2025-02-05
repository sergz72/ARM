#include "board.h"
#include "spi_commands.h"
#include <shell.h>
#include <stdlib.h>
#include <read_hex_string.h>
#include <string.h>

static int spi_transfer_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem spiwrite_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, spi_transfer_handler}
};
static const ShellCommand spi_transfer_command = {
    spiwrite_command_items,
    "spi_trfr",
    "spi_trfr data"
};

static int spibaud_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem spibaud_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, spibaud_handler}
};
static const ShellCommand spibaud_command = {
    spibaud_command_items,
    "spi_baud",
    "spi_baud speed_in_khz"
};

static int spibaud_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int baud = atoi(argv[0]);
  if (baud <= 0)
  {
    pfunc("Invalid baud rate\n");
    return 1;
  }
  baud = spi_set_baudrate(baud * 1000);
  pfunc("spi baud rate has been set to %dHz\n", baud);
  return 0;
}

static int spi_transfer_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *_data)
{
  int i, num_bytes;
  static unsigned char data[100], rdata[100];

  num_bytes = read_hex_string(argv[0], data, sizeof data);
  if (num_bytes <= 0)
  {
    pfunc("Invalid send_data\n");
    return 1;
  }
  pfunc("spi transfer %d bytes\ndata: ", num_bytes);
  for (i = 0; i < num_bytes; i++)
    pfunc("%02X", data[i]);
  pfunc("\n");
  spi_transfer(data, num_bytes, rdata);
  pfunc("read data: ");
  for (i = 0; i < num_bytes; i++)
    pfunc("%02X", rdata[i]);
  pfunc("\n");
  return 0;
}

void register_spi_commands(void)
{
  shell_register_command(&spi_transfer_command);
  shell_register_command(&spibaud_command);
}
