#include <dev_ina226.h>
#include <stddef.h>
#include <ina226.h>
#include <stdlib.h>
#include <string.h>

static const INA226Config dcfg = {
  .vshct = INA_VSHCT_1100,
  .vbusct = INA_VBUSCT_1100,
  .awg = INA_AVG_256,
  .mode = INA_MODE_SHUNT_BUS_TRIG
};

void* ina226_initializer(int idx, void **data)
{
  DEV_INA226Config* cfg = malloc(sizeof(DEV_INA226Config));
  if (cfg)
    cfg->r = 100; //mOhm
  *data = malloc(sizeof(DEV_INA226Data));
  return cfg;
}

int ina226_timer_event(int idx, int step, void *config, void *data, unsigned char *buffer)
{
  int rc;
  DEV_INA226Data *ddata = (DEV_INA226Data*)data;
  DEV_INA226Config* cfg = (DEV_INA226Config*)config;

  if (step == 8)
    ina226SetConfig(idx, INA226_DEVICE_ID, &dcfg);
  if (step != 9)
    return 0;

  if (data)
  {
    rc = ina226GetBusVoltage(idx, INA226_DEVICE_ID, &ddata->voltage);
    if (rc)
      ddata->voltage = 0;
    rc = ina226GetShuntCurrent(idx, INA226_DEVICE_ID, cfg->r, &ddata->current);
    if (rc)
      ddata->current = 0;
    memcpy(buffer, ddata, 8);
    return 8;
  }
  return 0;
}
