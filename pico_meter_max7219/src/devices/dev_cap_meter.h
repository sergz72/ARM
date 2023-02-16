#ifndef _DEV_CAP_METER_H
#define _DEV_CAP_METER_H

#include <shell.h>

void* cap_meter_initializer(void);
void *cap_meter_data_collector(int step, void* config, void *data);
void cap_meter_ui_handler(void* data, void* config);
int cap_meter_print_config(printf_func pfunc, void *device_config);
int cap_meter_set_config(printf_func pfunc, int argc, char **argv, void *device_config);

#endif
