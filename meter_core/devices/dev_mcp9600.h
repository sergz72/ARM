#ifndef _MAX7219_DEV_MCP9600_H
#define _MAX7219_DEV_MCP9600_H

#include <devices.h>

#define MCP9600_DEVICE_ID 0xCE

void mcp9600_initializer(DeviceObject *o);
int mcp9600_timer_event(DeviceObject *o, int step, unsigned char *buffer);
int mcp9600_save_config(DeviceObject *o, void *buffer);

#endif
