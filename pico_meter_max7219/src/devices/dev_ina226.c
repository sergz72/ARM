#include <dev_ina226.h>
#include <stddef.h>
#include <ina226.h>
#include <stdlib.h>
#include "ui.h"

static const INA226Config dcfg = {
  .vshct = INA_VSHCT_1100,
  .vbusct = INA_VBUSCT_1100,
  .awg = INA_AVG_256,
  .mode = INA_MODE_SHUNT_BUS_TRIG
};

void* ina226_initializer(void)
{
  if (!ina226SetConfig(0, INA226_DEVICE_ID, &dcfg))
  {
    DEV_INA226Config *cfg = malloc(sizeof(DEV_INA226Config));
    if (cfg)
      cfg->r = 100; //mOhm
    return cfg;
  }
  return NULL;
}

void *ina226_data_collector(int step, void *config, void *prev_data)
{
  int rc;
  DEV_INA226Data *data;
  DEV_INA226Config* cfg;

  if (step != 9)
    return prev_data;

  cfg = (DEV_INA226Config*)config;
  data = malloc(sizeof(DEV_INA226Data));
  if (data)
  {
    rc = ina226GetBusVoltage(0, INA226_DEVICE_ID, &data->voltage);
    if (rc)
      data->voltage = 0;
    rc = ina226GetShuntCurrent(0, INA226_DEVICE_ID, cfg->r, &data->current);
    if (rc)
      data->current = 0;
    data->current /= 100;
    data->power = abs(data->current) / 10 * abs(data->voltage) / 1000;
  }
  ina226SetConfig(0, INA226_DEVICE_ID, &dcfg);
  return data;
}

void ina226_ui_init_handler(void* config)
{
  LED_ClearScreen();
}

void ina226_ui_handler(void* data, void* config)
{
  char sign;
  DEV_INA226Data* d = (DEV_INA226Data*)data;

  if (d->current < 0)
  {
    sign = '-';
    d->current = -d->current;
  }
  else
    sign = ' ';
  LED_Printf(0, 0, "U%7d", d->voltage);
  LED_Printf(1, 0x40, "I%c%5d%d", sign, d->current / 10, d->current % 10);
  LED_Printf(2, 0, "P%7u", d->power);
}
