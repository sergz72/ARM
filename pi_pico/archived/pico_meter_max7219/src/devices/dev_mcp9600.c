#include "dev_mcp9600.h"
#include "ui.h"
#include "dev_keyboard.h"
#include <mcp9600.h>
#include <malloc.h>

#define TEMP_ERROR (-100000)

typedef struct {
  MCP9600DeviceConfig dcfg;
  MCP9600SensorConfig scfg;
} DEV_MCP9600Config;

typedef struct {
  int th, delta, tc;
  unsigned char status;
} DEV_MCP9600Data;

const char thermocouple_type[] = {
    'K','J','t', 'n','S','E','B','r'
};

void* mcp9600_initializer(void)
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

static void ShowMenu(DEV_MCP9600Config *cfg)
{
  if (cfg)
  {
    LED_Printf(3, 0, "%c F%d%d%s",
               thermocouple_type[cfg->scfg.thermocouple_type],
               cfg->scfg.filter_coefficient,
               18 - cfg->dcfg.adc_resolution * 2,
               cfg->dcfg.low_cold_juction_resolution ? "LO" : "HI");
  }
}

void mcp9600_ui_init_handler(void* config)
{
  LED_ClearScreen();
  ShowMenu((DEV_MCP9600Config *)config);
}

void *mcp9600_data_collector(int step, void* config, void *data)
{
  DEV_MCP9600Data *d;
  unsigned char status;
  switch (step)
  {
    case 9:
      if (mcp9600GetStatus(0, MCP9600_DEVICE_ID, &status))
        return NULL;
      d = malloc(sizeof(DEV_MCP9600Data));
      if (d)
      {
        d->status = status;
        if (!(status & (MCP9600_STATUS_RANGE|MCP9600_STATUS_SHORT_CIRCUIT)))
        {
          if (mcp9600GetHotJunctionTemperature(0, MCP9600_DEVICE_ID, &d->th))
            d->th = TEMP_ERROR;
          if (mcp9600GetDeltaTemperature(0, MCP9600_DEVICE_ID, &d->delta))
            d->delta = TEMP_ERROR;
          if (mcp9600GetColdJunctionTemperature(0, MCP9600_DEVICE_ID, &d->tc))
            d->tc = TEMP_ERROR;
        }
        return d;
      }
      break;
  }

  return NULL;
}

static void ReportError(const char *message)
{
  LED_Write_String(0, 0, message);
  LED_Write_String(1, 0, message);
  LED_Write_String(2, 0, message);
}

static void ReportTemperature(int line, char symbol, int temperature)
{
  char sign;
  if (temperature == TEMP_ERROR)
    LED_Printf(line, 0, "%c TEMPER", symbol);
  else
  {
    sign = ' ';
    if (temperature < 0)
    {
      sign = '-';
      temperature = -temperature;
    }
    LED_Printf(line, 0x20, "%c%c%4d%02d", symbol, sign, temperature / 100, temperature % 100);
  }
}

void mcp9600_ui_handler(void* data, void* config)
{
  DEV_MCP9600Data *d = (DEV_MCP9600Data*)data;
  if (!d)
  {
    ReportError("   ERROR");
    return;
  }
  if (d->status & MCP9600_STATUS_RANGE)
  {
    ReportError("   RANGE");
    return;
  }
  if (d->status & MCP9600_STATUS_SHORT_CIRCUIT)
  {
    ReportError("   SHORT");
    return;
  }
  ReportTemperature(0, 'H', d->th);
  ReportTemperature(1, 'd', d->delta);
  ReportTemperature(2, 'C', d->tc);
}

static void SelectNextThermocoupleType(DEV_MCP9600Config *cfg)
{
  cfg->scfg.thermocouple_type = (cfg->scfg.thermocouple_type + 1) & 7;
  mcp9600SetSensorConfig(0, MCP9600_DEVICE_ID, &cfg->scfg);
}

static void SelectNextFilter(DEV_MCP9600Config *cfg)
{
  cfg->scfg.filter_coefficient = (cfg->scfg.filter_coefficient + 1) & 7;
  mcp9600SetSensorConfig(0, MCP9600_DEVICE_ID, &cfg->scfg);
}

static void SelectNextAdcResolution(DEV_MCP9600Config *cfg)
{
  cfg->dcfg.adc_resolution = (cfg->dcfg.adc_resolution + 1) & 3;
  mcp9600SetDeviceConfig(0, MCP9600_DEVICE_ID, &cfg->dcfg);
}

static void SelectNextColdJunctionResolution(DEV_MCP9600Config *cfg)
{
  cfg->dcfg.low_cold_juction_resolution = ~cfg->dcfg.low_cold_juction_resolution;
  mcp9600SetDeviceConfig(0, MCP9600_DEVICE_ID, &cfg->dcfg);
}

int mcp9600_ui_keyboard_handler(void *config, unsigned int event)
{
  switch (event)
  {
    case KEYBOARD_EVENT_F1:
      SelectNextThermocoupleType((DEV_MCP9600Config *)config);
      ShowMenu((DEV_MCP9600Config *)config);
      return 1;
    case KEYBOARD_EVENT_F2:
      SelectNextFilter((DEV_MCP9600Config *)config);
      ShowMenu((DEV_MCP9600Config *)config);
      return 1;
    case KEYBOARD_EVENT_F3:
      SelectNextAdcResolution((DEV_MCP9600Config *)config);
      ShowMenu((DEV_MCP9600Config *)config);
      return 1;
    case KEYBOARD_EVENT_F4:
      SelectNextColdJunctionResolution((DEV_MCP9600Config *)config);
      ShowMenu((DEV_MCP9600Config *)config);
      return 1;
    default:
      return 0;
  }
}
