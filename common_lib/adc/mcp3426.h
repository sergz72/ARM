#ifndef _MCP3426_H
#define _MCP3426_H

typedef struct {
  unsigned char start_conversion;
  unsigned char channel;
  unsigned char continuous_conversion;
  unsigned char sample_rate;
  unsigned char gain;
} MCP3426Config;

#define MCP3426_GAIN_1 0
#define MCP3426_GAIN_2 1
#define MCP3426_GAIN_4 2
#define MCP3426_GAIN_8 3

#define MCP3426_RATE_240 0
#define MCP3426_RATE_60  (1 << 2)
#define MCP3426_RATE_15  (2 << 2)

#define MCP3426_CHANNEL_1 0
#define MCP3426_CHANNEL_2 0x20
#define MCP3426_CHANNEL_3 0x40
#define MCP3426_CHANNEL_4 0xC0

int mcp3426Read(int channel, unsigned char address, unsigned char *data, unsigned int l);
int mcp3426Write(int channel, unsigned char address, unsigned char data);
int mcp3426SetConfig(int channel, unsigned char address, const MCP3426Config *cfg);
int mcp3426GetVoltage(int channel, unsigned char address, int *voltage);

#endif
