#ifndef _DEVICES_H
#define _DEVICES_H

#define MAX_DEVICES 16

#include <shell.h>

typedef struct {
  const char *name, *short_name;
  void* (*initializer)(void);
  void* (*data_collector)(int step, void *config, void *data);
  void (*ui_init_handler)(void *config);
  void (*ui_data_handler)(void* data, void *config);
  void (*ui_keyboard_handler)(void *config, unsigned int event);
  int (*print_config)(printf_func pfunc, void *device_config);
  int (*set_config)(printf_func pfunc, int argc, char **argv, void *device_config);
  unsigned int set_config_parameter_count;
  const char *set_config_help;
  unsigned char *menu;
} Device;

extern const Device *device_list[MAX_DEVICES];
extern void *device_data[MAX_DEVICES];
extern void* device_config[MAX_DEVICES];

void BuildDeviceList(void);
void BuildShellCommands(void);
void BuildDeviceData(int step);

extern int found_devices;

#endif

