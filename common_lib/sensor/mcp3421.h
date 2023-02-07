#ifndef _MCP3421_H
#define _MCP3421_H

typedef struct {
  unsigned char start_conversion;
  unsigned char continuous_conversion;
  unsigned char sample_rate;
  unsigned char gain;
} MCP3421Config;

#define MCP3421_GAIN_1 0
#define MCP3421_GAIN_2 1
#define MCP3421_GAIN_4 2
#define MCP3421_GAIN_8 3

#define MCP3421_RATE_240 0
#define MCP3421_RATE_60  (1 << 2)
#define MCP3421_RATE_15  (2 << 2)
#define MCP3421_RATE_4   (3 << 2)

int mcp3421Read(int channel, unsigned char address, unsigned char *data, unsigned int l);
int mcp3421Write(int channel, unsigned char address, unsigned char data);
int mcp3421SetConfig(int channel, unsigned char address, const MCP3421Config *cfg);
int mcp3421Get18BitVoltage(int channel, unsigned char address, int *voltage);
int mcp3421Get16BitVoltage(int channel, unsigned char address, int *voltage);

#endif
