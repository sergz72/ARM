#include "board.h"
#include "sht_commands.h"
#include <shell.h>
#include <sht4x.h>

static int sht_init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem sht_init_command_items[] = {
  {nullptr, nullptr, sht_init_handler}
};
static const ShellCommand sht_init_command = {
  sht_init_command_items,
  "sht_init",
  "sht_init",
  nullptr,
  nullptr
};

static int sht_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem sht_get_command_items[] = {
  {nullptr, nullptr, sht_get_handler}
};
static const ShellCommand sht_get_command = {
  sht_get_command_items,
  "sht_get",
  "sht_get",
  nullptr,
  nullptr
};

static int sht_get_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  short temperature;
  unsigned short humidity;
  unsigned int rc = sht4x_measure(&temperature, &humidity);
  if (rc)
    return (int)rc;
  pfunc("temperature: %d\nhumidity: %d\n", temperature, humidity);
  return 0;
}

static int sht_init_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  return sht4x_init_sensor();
}

void register_sht_commands(void)
{
  shell_register_command(&sht_init_command);
  shell_register_command(&sht_get_command);
}
