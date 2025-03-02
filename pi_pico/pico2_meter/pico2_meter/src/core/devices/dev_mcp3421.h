#ifndef _DEV_MCP3421_H_
#define _DEV_MCP3421_H_

#include <devices.h>

#define MCP3421_DEVICE_ID 0xD0

void mcp3421_initializer(DeviceObject *o);
int mcp3421_timer_event(DeviceObject *o, int step, int interrupt, unsigned char *buffer);
int mcp3421_save_config(DeviceObject *o, void *buffer);

#endif

