#include "devices.h"
#include <stddef.h>
#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include "device_list.h"
#include "board.h"
#include "dev_keyboard.h"
#include "settings.h"
#include <stdlib.h>

typedef struct {
  int (*print_config)(printf_func pfunc, void *device_config);
  int idx;
} get_config_command_data;

typedef struct {
  int (*set_config)(printf_func pfunc, int argc, char **argv, void *device_config);
  int idx;
} set_config_command_data;

static int get_config_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem print_config_command_items[] = {
    {NULL, NULL, get_config_handler}
};
static const ShellCommand print_config_command = {
    print_config_command_items,
    "config_get",
    NULL
};

static int commit_config_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem commit_config_command_items[] = {
    {NULL, NULL, commit_config_handler}
};
static const ShellCommand commit_config_command = {
    commit_config_command_items,
    "config_commit",
    NULL
};

static int set_config_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data);
static const ShellCommandItem set_config_command_items[] = {
    {NULL, param_handler, NULL},
    {NULL, param_handler, NULL},
    {NULL, NULL, set_config_handler}
};
static const ShellCommand set_config_command = {
    set_config_command_items,
    "config_set",
    "config_set name value"
};

Device *device_list[MAX_DEVICES];
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
      device_list[found_devices] = (Device*)d;
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

static int get_device_config_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  get_config_command_data *d = (get_config_command_data *)data;
  if (d)
    return d->print_config(pfunc, device_config[d->idx]);
  return 1;
}

static int set_device_config_handler(printf_func pfunc, gets_func gfunc, int argc, char **argv, void *data)
{
  set_config_command_data *d = (set_config_command_data *)data;
  if (d)
    return d->set_config(pfunc, argc, argv, device_config[d->idx]);
  return 1;
}

static const ShellCommandItem *BuildCommandItems(unsigned int num_parameters,
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

static void BuildPrintDeviceConfigCommand(const Device* d, int idx)
{
  if (!d->print_config)
    return;
  get_config_command_data *data;
  ShellCommand *cmd = malloc(sizeof(ShellCommand));
  if (cmd)
  {
    cmd->name = malloc(strlen(d->name) + 5);
    if (cmd->name)
    {
      data = malloc(sizeof(get_config_command_data));
      if (data)
      {
        sprintf((char*)cmd->name, "%s_get", d->name);
        cmd->help = NULL;
        cmd->init = NULL;
        data->idx = idx;
        data->print_config = d->print_config;
        cmd->data = data;
        cmd->items = BuildCommandItems(0, get_device_config_handler);
        shell_register_command(cmd);
      }
    }
  }
}

static void BuildDeviceConfigCommand(config_type cfg_func, const char *name, const char *help, unsigned int parameter_count, const char * suffix, int idx)
{
  if (!cfg_func)
    return;
  set_config_command_data *data;
  ShellCommand *cmd = malloc(sizeof(ShellCommand));
  if (cmd)
  {
    cmd->name = malloc(strlen(name) + 5);
    cmd->help = malloc(strlen(name) + 6 + strlen(help));
    if (cmd->name && cmd->help)
    {
      data = malloc(sizeof(set_config_command_data));
      if (data)
      {
        sprintf((char*)cmd->name, "%s_%s", name, suffix);
        sprintf((char*)cmd->help, "%s %s", cmd->name, help);
        cmd->init = NULL;
        data->idx = idx;
        data->set_config = cfg_func;
        cmd->data = data;
        cmd->items = BuildCommandItems(parameter_count, set_device_config_handler);
        shell_register_command(cmd);
      }
    }
  }
}

static void BuildSetDeviceConfigCommand(const Device* d, int idx)
{
  BuildDeviceConfigCommand(d->set_config, d->name, d->set_config_help, d->set_config_parameter_count, "set", idx);
}

static void BuildDeviceCalibrateCommand(const Device* d, int idx)
{
  BuildDeviceConfigCommand(d->calibrate, d->name, d->calibrate_help, d->calibrate_parameter_count, "cal", idx);
}

static int get_config_handler(printf_func pfunc, gets_func gfunc, int argc, char** argv, void* data)
{
  int i;

  for (i = 0; i < SETTINGS_COUNT; i++)
    pfunc("%s %ld\n", setting_names[i], settings[i]);
  return 0;
}

static int set_config_handler(printf_func pfunc, gets_func gfunc, int argc, char** argv, void* data)
{
  int i;
  char* name = argv[0];
  long long int value = atol(argv[1]);

  for (i = 0; i < SETTINGS_COUNT; i++)
  {
    if (!strcmp(name, setting_names[i]))
    {
      settings[i] = value;
      return 0;
    }
  }

  pfunc("unknown setting name\n");
  return 1;
}

static int commit_config_handler(printf_func pfunc, gets_func gfunc, int argc, char** argv, void* data)
{
  return save_settings();
}

void BuildShellCommands(void)
{
  int i;
  const Device* d;

  shell_register_command(&print_config_command);
  shell_register_command(&set_config_command);
  shell_register_command(&commit_config_command);

  for (i = 0; i < found_devices; i++)
  {
    d = device_list[i];
    BuildPrintDeviceConfigCommand(d, i);
    BuildSetDeviceConfigCommand(d, i);
    BuildDeviceCalibrateCommand(d, i);
  }
}
