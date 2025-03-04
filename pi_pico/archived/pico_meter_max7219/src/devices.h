#ifndef _DEVICES_H
#define _DEVICES_H

#define MAX_DEVICES 16

#include <shell.h>

typedef int (*ui_keyboard_handler_type)(void *config, unsigned int event);
typedef int (*ui_encoder_handler_type)(void *config, int counter, int button_pressed);
typedef void (*ui_init_handler_type)(void *config);
typedef void (*ui_data_handler_type)(void* data, void *config);
typedef int (*config_type)(printf_func pfunc, int argc, char** argv, void* device_config);

typedef struct {
  const char *name, *short_name;
  void* (*initializer)(void);
  void* (*data_collector)(int step, void *config, void *data);
  ui_init_handler_type ui_init_handler;
  ui_data_handler_type ui_data_handler;
  ui_keyboard_handler_type ui_keyboard_handler;
  ui_encoder_handler_type ui_encoder_handler;
  int (*print_config)(printf_func pfunc, void *device_config);
  config_type set_config;
  unsigned int set_config_parameter_count;
  const char* set_config_help;
  config_type calibrate;
  unsigned int calibrate_parameter_count;
  const char* calibrate_help;
} Device;

extern Device *device_list[MAX_DEVICES];
extern void *device_data;
extern void* device_config[MAX_DEVICES];

void BuildDeviceList(void);
void BuildShellCommands(void);
void BuildDeviceData(int step);

extern int found_devices;

#endif

