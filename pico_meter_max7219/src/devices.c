#include "devices.h"
#include <stddef.h>
#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include "device_list.h"
#include "board.h"
#include "dev_keyboard.h"

typedef struct {
  int (*print_config)(printf_func pfunc, void *device_config);
  int idx;
} get_config_command_data;

typedef struct {
  int (*set_config)(printf_func pfunc, int argc, char **argv, void *device_config);
  int idx;
} set_config_command_data;

const Device *device_list[MAX_DEVICES];
void *device_data;
void* device_config[MAX_DEVICES];
int found_devices;

void BuildDeviceList(void)
{
  int i;
  const Device* d;
  void *config;

  d = devices;
  found_devices = 0;
  for (i = 0; i < MAX_KNOWN_DEVICES; i++)
  {
    config = d->initializer();
    if (config)
    {
      device_list[found_devices] = d;
      device_config[found_devices++] = config;
    }
    d++;
  }
}

void BuildDeviceData(int step)
{
  const Device* d;

  if (current_keyboard_device >= 0)
  {
    d = device_list[current_keyboard_device];
    if (d->data_collector)
      device_data = d->data_collector(step, device_config[current_keyboard_device], device_data);
    else
      device_data = NULL;
  }
}

static int get_config_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  get_config_command_data *d = (get_config_command_data *)data;
  if (d)
    return d->print_config(pfunc, device_config[d->idx]);
  return 1;
}

static int set_config_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  set_config_command_data *d = (set_config_command_data *)data;
  if (d)
    return d->set_config(pfunc, argc, argv, device_config[d->idx]);
  return 1;
}

static const ShellCommandItem *BuildCommandItems(int idx, unsigned int num_parameters,
                                                 int (*final_handler)(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data))
{
  ShellCommandItem *items = malloc(sizeof(ShellCommandItem) * (1 + num_parameters));
  ShellCommandItem *result = items;
  if (items)
  {
    while (num_parameters > 0)
    {
      items->text = NULL;
      items->final_handler = NULL;
      items->handler = param_handler;
      items++;
      num_parameters--;
    }
    items->text = NULL;
    items->handler = NULL;
    items->final_handler = final_handler;
  }
  return result;
}

static void BuildPrintConfigCommand(const Device* d, int idx)
{
  if (!d->print_config)
    return;
  get_config_command_data *data;
  ShellCommand *cmd = malloc(sizeof(ShellCommand));
  if (cmd)
  {
    cmd->name = malloc(strlen(d->name) + 6);
    if (cmd->name)
    {
      data = malloc(sizeof(get_config_command_data));
      if (data)
      {
        sprintf((char*)cmd->name, "%s%d_get", d->name, idx);
        cmd->help = NULL;
        cmd->init = NULL;
        data->idx = idx;
        data->print_config = d->print_config;
        cmd->data = data;
        cmd->items = BuildCommandItems(idx, 0, get_config_handler);
        shell_register_command(cmd);
      }
    }
  }
}

static void BuildSetConfigCommand(const Device* d, int idx)
{
  if (!d->set_config)
    return;
  set_config_command_data *data;
  ShellCommand *cmd = malloc(sizeof(ShellCommand));
  if (cmd)
  {
    cmd->name = malloc(strlen(d->name) + 6);
    cmd->help = malloc(strlen(d->name) + 7 + strlen(d->set_config_help));
    if (cmd->name && cmd->help)
    {
      data = malloc(sizeof(set_config_command_data));
      if (data)
      {
        sprintf((char*)cmd->name, "%s%d_set", d->name, idx);
        sprintf((char*)cmd->help, "%s %s", cmd->name, d->set_config_help);
        cmd->help = NULL;
        cmd->init = NULL;
        data->idx = idx;
        data->set_config = d->set_config;
        cmd->data = data;
        cmd->items = BuildCommandItems(idx, d->set_config_parameter_count, set_config_handler);
        shell_register_command(cmd);
      }
    }
  }
}

void BuildShellCommands(void)
{
  int i;
  const Device* d;

  for (i = 0; i < found_devices; i++)
  {
    d = device_list[i];
    BuildPrintConfigCommand(d, i);
    BuildSetConfigCommand(d, i);
  }
}
