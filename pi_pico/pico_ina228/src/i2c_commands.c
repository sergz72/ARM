#include "board.h"
#include "i2c_commands.h"
#include <shell.h>

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
    delayms(1);
  }

  return 0;
}

void register_i2c_commands(void)
{
  shell_register_command(&scan_command);
}
