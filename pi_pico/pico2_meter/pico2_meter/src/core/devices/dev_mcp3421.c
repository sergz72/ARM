#include "dev_mcp3421.h"
#include <mcp3421.h>
#include <stdlib.h>
#include <string.h>
#include "device_config.h"

typedef struct {
  int voltage;
} DEV_MCP3421Data;

typedef struct {
  int koef;
  int offset;
} DEV_MCP3421Config;

static const MCP3421Config dcfg = {
  .start_conversion = 1,
  .continuous_conversion = 0,
  .sample_rate = MCP3421_RATE_4,
  .gain = MCP3421_GAIN_1
};

static const MeterConfig config = {
  .channels = 1,
  .resolution = 4, // 32 bit
  .measure_type = MEASURE_TYPE_VOLTS,
  .value_type = VALUE_TYPE_MICROS,
  .numbers_before_point = 2
};

#define isValidCoef(x) (x >= 50000 && x <= 150000)
#define isValidOffset(x) (x >= -50000 && x <= 50000)

void mcp3421_initializer(DeviceObject *o)
{
  DEV_MCP3421Config* cfg = malloc(sizeof(DEV_MCP3421Config));
  if (cfg)
  {
    if (read_config_from_eeprom(o->idx, cfg, sizeof(DEV_MCP3421Config)) ||
        !isValidCoef(cfg->koef) || !isValidOffset(cfg->offset))
    {
      cfg->koef = 116200;
      cfg->offset = -1510;
    }
  }
  o->device_config = cfg;
}

int mcp3421_timer_event(DeviceObject *o, int step, int interrupt, unsigned char *buffer)
{
  DEV_MCP3421Config* cfg = (DEV_MCP3421Config*)o->device_config;
  int v;

  switch (step)
  {
    case 0:
      mcp3421SetConfig(o->idx, MCP3421_DEVICE_ID, &dcfg);
      break;
    case 9:
      mcp3421Get18BitVoltage(o->idx, MCP3421_DEVICE_ID, &v);
      int voltage = (int)((long long int)v * 4096 * cfg->koef / (0x3FFFF * 10)) + cfg->offset;
      memcpy(buffer, &voltage, 4);
      return 4;
    default:
      break;
  }
  return 0;
}

int mcp3421_save_config(DeviceObject *o, void *buffer)
{
  return BuildMeterConfig(buffer, &config, "MCP3421");
}
