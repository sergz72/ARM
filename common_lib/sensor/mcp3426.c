#include <mcp3426.h>

int mcp3426SetConfig(int channel, unsigned char address, const MCP3426Config *cfg)
{
  unsigned char c = cfg->gain|cfg->sample_rate|cfg->channel;
  if (cfg->continuous_conversion)
    c |= 0x10;
  if (cfg->start_conversion)
    c |= 0x80;
  return mcp3426Write(channel, address, c);
}

int mcp3426GetVoltage(int channel, unsigned char address, int *voltage)
{
  unsigned char data[2];
  int v;
  int rc = mcp3426Read(channel, address, data, 2);
  if (!rc)
  {
    v = (data[0] << 8) | data[1];
    if (v & 0x8000)
      v |= 0xFFFF0000;
    *voltage = v;
  }
  return rc;
}
