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

static const PowerMeterConfig config = {
  .channels = 1,
  .voltage_resolution = 4,
  .voltage_value_type = VALUE_TYPE_MILLIS,
  .voltage_numbers_before_point = 2,
  .current_resolution = 4,
  .current_value_type = VALUE_TYPE_MICROS,
  .current_numbers_before_point = 1
};

void ina226_initializer(DeviceObject *o)
{
  DEV_INA226Config* cfg = malloc(sizeof(DEV_INA226Config));
  if (cfg)
  {
    cfg->r = 100; //mOhm
    o->device_data = malloc(sizeof(DEV_INA226Data));
  }
  o->device_config = cfg;
}

int ina226_timer_event(DeviceObject *o, int step, unsigned char *buffer)
{
  int rc;
  DEV_INA226Data *ddata = (DEV_INA226Data*)o->device_data;
  DEV_INA226Config* cfg = (DEV_INA226Config*)o->device_config;

  if (step == 8)
    ina226SetConfig(o->idx, INA226_DEVICE_ID, &dcfg);
  if (step != 9)
    return 0;

  if (ddata)
  {
    rc = ina226GetBusVoltage(o->idx, INA226_DEVICE_ID, &ddata->voltage);
    if (rc)
      ddata->voltage = 0;
    rc = ina226GetShuntCurrent(o->idx, INA226_DEVICE_ID, cfg->r, &ddata->current);
    if (rc)
      ddata->current = 0;
    memcpy(buffer, ddata, 8);
    return 8;
  }
  return 0;
}

int ina226_save_config(DeviceObject *o, void *buffer)
{
  return BuildPowerMeterConfig(buffer, &config, "INA226");
}
