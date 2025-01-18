#ifndef _MAX7219_DEV_MCP9600_H
#define _MAX7219_DEV_MCP9600_H

#define MCP9600_DEVICE_ID 0xCE

void* mcp9600_initializer(int idx, void **data);
int mcp9600_timer_event(int idx, int step, void *config, void *data, unsigned char *buffer);

#endif
