#include "ad9833_commands.h"
#include "board.h"
#include <shell.h>
#include <mcp41_42.h>
#include <ad9833.h>
#include <stdlib.h>
#include <string.h>

unsigned short ad9833_cfg;

static int ad9833freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem ad9833freq_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, ad9833freq_handler}
};
static const ShellCommand ad9833freq_command = {
    ad9833freq_command_items,
    "ad9833freq",
    "ad9833freq frequency"
};

static int ad9833mode_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem ad9833mode_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, ad9833mode_handler}
};
static const ShellCommand ad9833mode_command = {
    ad9833mode_command_items,
    "ad9833mode",
    "ad9833mode sin|tri|msb|msb2"
};

static int ad9833att_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem ad9833att_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, NULL, ad9833att_handler}
};
static const ShellCommand ad9833att_command = {
    ad9833att_command_items,
    "ad9833att",
    "ad9833att value"
};

static int ad9833enable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem ad9833enable_command_items[] = {
    {NULL, NULL, ad9833enable_handler}
};
static const ShellCommand ad9833enable_command = {
    ad9833enable_command_items,
    "ad9833enable",
    NULL
};

static int ad9833disable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv);
static const ShellCommandItem ad9833disable_command_items[] = {
    {NULL, NULL, ad9833disable_handler}
};
static const ShellCommand ad9833disable_command = {
    ad9833disable_command_items,
    "ad9833disable",
    NULL
};

static int ad9833freq_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  unsigned int code;
  int freq = atoi(argv[0]);
  if (freq > 0 && freq <= AD9833_MCLK_FREQ / 2)
  {
    code = (unsigned int)(((unsigned long long int)freq << AD9833_ACCUMULATOR_BITS) / AD9833_MCLK_FREQ);
    pfunc("freq = %d, freq word = %d\n", freq, code);
    ad9833_set_freq0_word(0, code);
    return 0;
  }
  pfunc("Invalid frequency\n");
  return 1;
}

static int ad9833mode_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  if (!strcmp(argv[0], "sin"))
  {
    ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_SINUS);
    return 0;
  }
  if (!strcmp(argv[0], "tri"))
  {
    ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_TRIANGLE);
    return 0;
  }
  if (!strcmp(argv[0], "msb"))
  {
    ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_MSB);
    return 0;
  }
  if (!strcmp(argv[0], "msb2"))
  {
    ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_MSBDIV2);
    return 0;
  }
  pfunc("Invalid mode\n");
  return 1;
}

static int ad9833enable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  ad9833_cfg &= ~(AD9833_CFG_SLEEP1|AD9833_CFG_SLEEP12);
  ad9833_set_config(0, ad9833_cfg);
  return 0;
}

static int ad9833disable_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  ad9833_cfg |= AD9833_CFG_SLEEP1|AD9833_CFG_SLEEP12;
  ad9833_set_config(0, ad9833_cfg);
  return 0;
}

static int ad9833att_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv)
{
  int code = atoi(argv[0]);
  if (code < 0 || code > 255)
  {
    pfunc("Invalid value\n");
    return 1;
  }
  mcp41_42_set(0, MCP41_42_SET_DATA0, code);
  return 0;
}

void register_ad9833_commands(void)
{
  ad9833_cfg = AD9833_CFG_SLEEP1|AD9833_CFG_SLEEP12;
  shell_register_command(&ad9833freq_command);
  shell_register_command(&ad9833mode_command);
  shell_register_command(&ad9833enable_command);
  shell_register_command(&ad9833disable_command);
  shell_register_command(&ad9833att_command);
}
