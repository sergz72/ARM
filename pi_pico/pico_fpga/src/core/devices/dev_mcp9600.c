#include "dev_mcp9600.h"
#include <devices.h>
#include <mcp9600.h>
#include <malloc.h>
#include <string.h>

#define TEMP_ERROR (-100000)

typedef struct {
  MCP9600DeviceConfig dcfg;
  MCP9600SensorConfig scfg;
} DEV_MCP9600Config;

typedef struct {
  int th, delta, tc;
} DEV_MCP9600Data;

const char thermocouple_type[] = {
    'K','J','t', 'n','S','E','B','r'
};

static const MeterConfig config = {
  .channels = 3,
  .resolution = 4, // 32 bit
  .measure_type = MEASURE_TYPE_GRADS,
  .value_type = VALUE_TYPE_MILLIS,
  .numbers_before_point = 4
};

void* mcp9600_initializer(int idx, void **data)
{
  unsigned short id;
  if (!mcp9600ReadDeviceID(0, MCP9600_DEVICE_ID, &id))
  {
    DEV_MCP9600Config *cfg = malloc(sizeof(DEV_MCP9600Config));
    if (cfg)
    {
      cfg->scfg.thermocouple_type = MCP9600_THERMOCOUPLE_TYPE_K;
      cfg->scfg.filter_coefficient = 0;
      cfg->dcfg.burst_mode_samples = MCP9600_BURST_MODE_1;
      cfg->dcfg.adc_resolution = MCP9600_ADC_RESOLUTION_18;
      cfg->dcfg.mode = MCP9600_MODE_NORMAL;
      cfg->dcfg.low_cold_juction_resolution = 0;
      mcp9600SetSensorConfig(0, MCP9600_DEVICE_ID, &cfg->scfg);
      mcp9600SetDeviceConfig(0, MCP9600_DEVICE_ID, &cfg->dcfg);
    }
    return cfg;
  }
  return NULL;
}

int mcp9600_timer_event(int idx, int step, void *config, void *data, int interrupt, unsigned char *buffer)
{
  DEV_MCP9600Data d;
  unsigned char status;

  switch (step)
  {
    case 9:
      if (mcp9600GetStatus(0, MCP9600_DEVICE_ID, &status) ||
          status & (MCP9600_STATUS_RANGE|MCP9600_STATUS_SHORT_CIRCUIT))
        d.th = d.delta = d.tc = TEMP_ERROR;
      else
      {
        if (mcp9600GetHotJunctionTemperature(0, MCP9600_DEVICE_ID, &d.th))
          d.th = TEMP_ERROR;
        if (mcp9600GetDeltaTemperature(0, MCP9600_DEVICE_ID, &d.delta))
          d.delta = TEMP_ERROR;
        if (mcp9600GetColdJunctionTemperature(0, MCP9600_DEVICE_ID, &d.tc))
          d.tc = TEMP_ERROR;
      }
      memcpy(buffer, &d, sizeof(DEV_MCP9600Data));
      return sizeof(DEV_MCP9600Data);
    default:
      break;
  }

  return 0;
}

int mcp9600_message_processor(unsigned char *buffer, int len)
{
  buffer[0] = 'e';
  return 1;
}

int mcp9600_save_config(void *buffer)
{
  return BuildMeterConfig(buffer, &config, "MCP9600");
}
