#ifndef _MCP41_42_H_
#define _MCP41_42_H_

#define MCP41_42_SET_DATA0 0x11
#define MCP41_42_SET_DATA1 0x12
#define MCP41_42_SHUTDOWN  0x20

void mcp41_42_set(int channel, unsigned char command, unsigned char data);

#endif
