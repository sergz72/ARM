#ifndef _DEV_MCP3421_H_
#define _DEV_MCP3421_H_

#define MCP3421_DEVICE_ID 0xD0

void* mcp3421_initializer(int idx, void **data);
int mcp3421_timer_event(int idx, int step, void* config, void *data, int interrupt, unsigned char *buffer);
int mcp3421_save_config(int idx, void *buffer);

#endif

