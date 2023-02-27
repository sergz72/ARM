#include "dev_mcp3421.h"
#include <mcp3421.h>
#include <stdlib.h>
#include <pico/time.h>
#include "device_config.h"
#include "ui.h"

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

void* mcp3421_initializer(void)
{
  if (!mcp3421SetConfig(0, MCP3421_DEVICE_ID, &dcfg))
  {
    DEV_MCP3421Config *cfg = malloc(sizeof(DEV_MCP3421Config));
    if (cfg)
    {
      if (read_config_from_eeprom(cfg, sizeof(DEV_MCP3421Config)) ||
          !isValidCoef(cfg->koef) || !isValidOffset(cfg->offset))
      {
        cfg->koef = 116200;
        cfg->offset = -1510;
      }
    }
    return cfg;
  }
  return NULL;
}

static int get_voltage(int koef, int offset)
{
  int v;
  mcp3421Get18BitVoltage(0, MCP3421_DEVICE_ID, &v);
  return (int)((long long int)v * 4096 * koef / (0x3FFFF * 100)) + offset;
}

void *mcp3421_data_collector(int step, void *config, void *prev_data)
{
  DEV_MCP3421Config* cfg = (DEV_MCP3421Config*)config;
  DEV_MCP3421Data *data;

  switch (step)
  {
    case 0:
      mcp3421SetConfig(0, MCP3421_DEVICE_ID, &dcfg);
      break;
    case 9:
      prev_data = malloc(sizeof(DEV_MCP3421Data));
      if (prev_data)
      {
        data = (DEV_MCP3421Data*)prev_data;
        data->voltage = get_voltage(cfg->koef, cfg->offset);
      }
      return prev_data;
  }
  return NULL;
}

void mcp3421_ui_handler(void* data, void* config)
{
  int v;
  char sign;
  DEV_MCP3421Data* d = (DEV_MCP3421Data*)data;

  v = d->voltage;
  if (v < 0)
  {
    sign = '-';
    v = -v;
  }
  else
    sign = ' ';
  LED_Printf(0, 4, "%c%2d%05d", sign, v / 100000, v % 100000);
}

int msp3421_print_config(printf_func pfunc, void *config)
{
  DEV_MCP3421Config* cfg = (DEV_MCP3421Config*)config;
  if (cfg)
  {
    pfunc("coef %d offset %d\n", cfg->koef, cfg->offset);
    return 0;
  }
  pfunc("NULL config\n");
  return 1;
}

int msp3421_set_config(printf_func pfunc, int argc, char **argv, void *config)
{
  DEV_MCP3421Config* cfg = (DEV_MCP3421Config*)config;
  int coef, offset;
  if (cfg)
  {
    coef = atoi(argv[0]);
    if (!isValidCoef(coef))
    {
      pfunc("Invalid coef\n");
      return 1;
    }
    offset = atoi(argv[1]);
    if (!isValidOffset(offset))
    {
      pfunc("Invalid offset\n");
      return 1;
    }
    cfg->koef = coef;
    cfg->offset = offset;
    pfunc("coef %d offset %d\n", cfg->koef, cfg->offset);
    return write_config_to_eeprom(cfg, sizeof(DEV_MCP3421Config));
  }
  pfunc("NULL config\n");
  return 1;
}

static int mcp3421_read(int koef, int offset)
{
  mcp3421SetConfig(0, MCP3421_DEVICE_ID, &dcfg);
  sleep_ms(300);
  return get_voltage(koef, offset);
}

static int offset_calibration(printf_func pfunc, void* device_config)
{
  int v, offset;
  DEV_MCP3421Config *cfg = (DEV_MCP3421Config *)device_config;
  offset = cfg->offset;
  pfunc("offset calibration\n");
  while (isValidOffset(offset))
  {
    v = mcp3421_read(cfg->koef, offset);
    pfunc("offset %d result %d\n", offset, v);
    if (v < 0)
      offset++;
    else
      break;
  }
  if (!isValidOffset(offset))
  {
    pfunc("calibration failed.\n");
    return 1;
  }
  while (isValidOffset(offset))
  {
    v = mcp3421_read(cfg->koef, offset);
    pfunc("offset %d result %d\n", offset, v);
    if (v > 0)
      offset--;
    else
      break;
  }
  if (!isValidOffset(offset))
  {
    pfunc("calibration failed.\n");
    return 1;
  }
  pfunc("offset calibration finished. Offset = %d\n", offset);
  cfg->offset = offset;
  return write_config_to_eeprom(cfg, sizeof(DEV_MCP3421Config));
}

static int coef_calibration(printf_func pfunc, int mv, void* device_config)
{
  int v, coef, above;
  DEV_MCP3421Config *cfg = (DEV_MCP3421Config *)device_config;
  coef = cfg->koef;
  pfunc("coef calibration\n");
  mv *= 100;
  while (isValidCoef(coef))
  {
    v = mcp3421_read(coef, cfg->offset);
    pfunc("coef %d result %d\n", coef, v);
    if (v <= mv)
      coef++;
    else
      break;
  }
  if (!isValidCoef(coef))
  {
    pfunc("calibration failed.\n");
    return 1;
  }
  above = coef;
  while (isValidCoef(coef))
  {
    coef--;
    v = mcp3421_read(coef, cfg->offset);
    pfunc("coef %d result %d\n", coef, v);
    if (v > mv)
      above = coef;
    else if (v < mv)
      break;
  }
  if (!isValidCoef(coef))
  {
    pfunc("calibration failed.\n");
    return 1;
  }
  coef = (above + coef) / 2;
  pfunc("coef calibration finished. Coef = %d\n", coef);
  cfg->koef = coef;
  return write_config_to_eeprom(cfg, sizeof(DEV_MCP3421Config));
}

int mcp3421_calibrate(printf_func pfunc, int argc, char** argv, void* device_config)
{
  int v;
  v = atoi(argv[0]);
  if (v < 0)
  {
    pfunc("Invalid value\n");
    return 1;
  }
  if (!v)
    return offset_calibration(pfunc, device_config);
  return coef_calibration(pfunc, v, device_config);
}
