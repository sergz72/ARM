#ifndef _DEV_MCP3421_H_
#define _DEV_MCP3421_H_

#define MCP3421_DEVICE_ID 0xD0

void* mcp3421_initializer(int idx);
void *mcp3421_data_collector(int idx, int step, void* config, void *data);

#endif

