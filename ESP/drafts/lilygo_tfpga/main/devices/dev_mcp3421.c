#include "dev_mcp3421.h"
#include <mcp3421.h>
#include <stdlib.h>
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

#define isValidCoef(x) (x >= 50000 && x <= 150000)
#define isValidOffset(x) (x >= -50000 && x <= 50000)

void* mcp3421_initializer(int idx)
{
  DEV_MCP3421Config* cfg = malloc(sizeof(DEV_MCP3421Config));
  if (cfg)
  {
    if (read_config_from_eeprom(idx, cfg, sizeof(DEV_MCP3421Config)) ||
        !isValidCoef(cfg->koef) || !isValidOffset(cfg->offset))
    {
      cfg->koef = 116200;
      cfg->offset = -1510;
    }
  }
  return cfg;
}

void *mcp3421_data_collector(int idx, int step, void *config, void *prev_data)
{
  DEV_MCP3421Config* cfg = (DEV_MCP3421Config*)config;
  DEV_MCP3421Data *data;
  int v;

  switch (step)
  {
    case 0:
      prev_data = malloc(sizeof(DEV_MCP3421Data));
      mcp3421SetConfig(idx, MCP3421_DEVICE_ID, &dcfg);
      break;
    case 9:
      data = (DEV_MCP3421Data *)prev_data;
      mcp3421Get18BitVoltage(idx, MCP3421_DEVICE_ID, &v);
      data->voltage = (int)((long long int)v * 4096 * cfg->koef / (0x3FFFF * 100)) + cfg->offset;
      break;
  }
  return prev_data;
}
