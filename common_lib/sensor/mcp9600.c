#include <mcp9600.h>

#define MCP9600_REG_TH 0
#define MCP9600_REG_DELTA 1
#define MCP9600_REG_TC 2
#define MCP9600_REG_STATUS 4
#define MCP9600_REG_SENSOR_CONFIG 5
#define MCP9600_REG_DEVICE_CONFIG 6
#define MCP9600_REG_ID 32

int mcp9600ReadDeviceID(int channel, unsigned char address, unsigned short *id)
{
  return mcp9600Read16(channel, address, MCP9600_REG_ID, id);
}

int mcp9600SetSensorConfig(int channel, unsigned char address, const MCP9600SensorConfig *cfg)
{
  return mcp9600Write(channel, address, MCP9600_REG_SENSOR_CONFIG, (cfg->filter_coefficient & 7) | (cfg->thermocouple_type << 4));
}

int mcp9600SetDeviceConfig(int channel, unsigned char address, const MCP9600DeviceConfig *cfg)
{
  unsigned char reg = (cfg->low_cold_juction_resolution ? 0x80 : 0) | cfg->mode | (cfg->adc_resolution << 5) | (cfg->burst_mode_samples << 2);
  return mcp9600Write(channel, address, MCP9600_REG_DEVICE_CONFIG, reg);
}

static int mcp9600GetTemperature(int channel, unsigned char address, unsigned char reg, int *temperature)
{
  short v;
  int rc = mcp9600Read16(channel, address, reg, (unsigned short*)&v);
  if (rc)
    return rc;
  *temperature = (int)v * 625 / 100;
  return 0;
}

int mcp9600GetHotJunctionTemperature(int channel, unsigned char address, int *temperature)
{
  return mcp9600GetTemperature(channel, address, MCP9600_REG_TH, temperature);
}

int mcp9600GetDeltaTemperature(int channel, unsigned char address, int *temperature)
{
  return mcp9600GetTemperature(channel, address, MCP9600_REG_DELTA, temperature);
}

int mcp9600GetColdJunctionTemperature(int channel, unsigned char address, int *temperature)
{
  return mcp9600GetTemperature(channel, address, MCP9600_REG_TC, temperature);
}

int mcp9600GetStatus(int channel, unsigned char address, unsigned char *status)
{
  return mcp9600Read8(channel, address, MCP9600_REG_STATUS, status);
}
