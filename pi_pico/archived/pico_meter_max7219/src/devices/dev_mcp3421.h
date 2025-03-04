#ifndef _DEV_MCP3421_H_
#define _DEV_MCP3421_H_

#include <shell.h>

#define MCP3421_DEVICE_ID 0xD0

void* mcp3421_initializer(void);
void *mcp3421_data_collector(int step, void* config, void *data);
void mcp3421_ui_handler(void* data, void* config);
int msp3421_print_config(printf_func pfunc, void *device_config);
int msp3421_set_config(printf_func pfunc, int argc, char **argv, void *device_config);
int mcp3421_calibrate(printf_func pfunc, int argc, char** argv, void* device_config);

#endif

