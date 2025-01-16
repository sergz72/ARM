#include <dev_ina226.h>
#include <stddef.h>
#include <ina226.h>
#include <stdlib.h>

static const INA226Config dcfg = {
  .vshct = INA_VSHCT_1100,
  .vbusct = INA_VBUSCT_1100,
  .awg = INA_AVG_256,
  .mode = INA_MODE_SHUNT_BUS_TRIG
};

void* ina226_initializer(int idx)
{
  DEV_INA226Config* cfg = malloc(sizeof(DEV_INA226Config));
  if (cfg)
    cfg->r = 100; //mOhm
  return cfg;
}

void *ina226_data_collector(int idx, int step, void *config, void *prev_data)
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
    rc = ina226GetBusVoltage(idx, INA226_DEVICE_ID, &data->voltage);
    if (rc)
      data->voltage = 0;
    rc = ina226GetShuntCurrent(idx, INA226_DEVICE_ID, cfg->r, &data->current);
    if (rc)
      data->current = 0;
    data->current /= 100;
    data->power = abs(data->current) / 10 * abs(data->voltage) / 1000;
    data->main_power = data->other_power = 0;
  }
  ina226SetConfig(idx, INA226_DEVICE_ID, &dcfg);
  return data;
}
