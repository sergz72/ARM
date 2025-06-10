#include "tlv320dac3100_commands.h"
#include "board.h"
#include <tlv320dac3100.h>
#include <shell.h>
#include <stdlib.h>
#include <string.h>
#include <audio.h>
#include "tlv.h"

static int param2_handler(const char *command, ShellCommandParameter **cp, const struct _ShellCommandItem **item);

static int tlv320dac3100_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int tlv320dac3100_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static int tlv320dac3100_handler3(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem tlv320dac3100_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param2_handler, tlv320dac3100_handler1},
    {NULL, param_handler, tlv320dac3100_handler2},
  {NULL, NULL, tlv320dac3100_handler3}
};
static const ShellCommand tlv320dac3100_command = {
  tlv320dac3100_command_items,
  "tlv",
  "tlv [volume dac value][clkout off|source div]"
};

static int param2_handler(const char *command, ShellCommandParameter **cp, const struct _ShellCommandItem **item)
{
  ShellCommandParameter *p = *cp;
  p--;
  if (p->length == 6 && !strncmp(command + p->offset, "volume", 6))
    return param_handler(command, cp, item);
  if (p->length == 6 && !strncmp(command + p->offset, "clkout", 6))
    return param_handler(command, cp, item);
  return 1;
}

static int tlv320dac3100_handler3(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int value = atoi(argv[2]);
  if (!strcmp(argv[0], "volume"))
  {
    int rc;
    if (!strcmp(argv[1], "dac"))
    {
      rc = tlv320dac3100_dac_volume(CHANNEL_LEFT, value);
      if (rc)
        return rc;
      return tlv320dac3100_dac_volume(CHANNEL_RIGHT, value);
    }
  }
  else if (!strcmp(argv[0], "clkout"))
  {
    int source = atoi(argv[1]);
    return tlv320dac3100_set_clkout(1, (unsigned char)source, (unsigned char)value);
  }

  pfunc("Invalid command parameter\n");
  return 1;
}

static int tlv320dac3100_handler2(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  if (!strcmp(argv[0], "clkout") && !strcmp(argv[1], "off"))
    return tlv320dac3100_set_clkout(0, 0, 1);

  pfunc("Invalid command parameter\n");
  return 1;
}

static int tlv320dac3100_handler1(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  int rc;

  pfunc("Invalid command parameter\n");
  return 1;
}

void register_tlv320dac3100_commands(void)
{
  shell_register_command(&tlv320dac3100_command);
}
