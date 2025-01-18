#include <dev_ina3221.h>
#include <dev_ina226.h>
#include <ina3221.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  DEV_INA226Data data[3];
} INA3221Data;

static const INA3221Config dcfg = {
    .channel_enable = {1, 1, 1},
    .base_config = {
        .vshct = INA_VSHCT_1100,
        .vbusct = INA_VBUSCT_1100,
        .awg = INA_AVG_256,
        .mode = INA_MODE_SHUNT_BUS_TRIG
    }
};

void* ina3221_initializer(int idx, void **data)
{
  DEV_INA226Config* cfg = malloc(sizeof(DEV_INA226Config));
  if (cfg)
    cfg->r = 100; //mOhm
  *data = calloc(1, sizeof(INA3221Data));
  return cfg;
}

static void collect_channel_data(int idx, int channel, int r, DEV_INA226Data *data)
{
  int rc = ina3221GetBusVoltage(idx, INA3221_DEVICE_ID, channel, &data->voltage);
  if (rc)
    data->voltage = 0;
  rc = ina3221GetShuntCurrent(idx, INA3221_DEVICE_ID, r, channel, &data->current);
  if (rc)
    data->current = 0;
}

int ina3221_timer_event(int idx, int step, void* config, void *data, unsigned char *buffer)
{
  INA3221Data *ddata = (INA3221Data*)data;
  DEV_INA226Config* cfg = (DEV_INA226Config*)config;

  if (data && cfg)
  {
    if (step == 0)
      ina3221SetConfig(idx, INA3221_DEVICE_ID, &dcfg);
    else if (step <= 3)
    {
      collect_channel_data(idx, step - 1, cfg->r, &ddata->data[step - 1]);
      if (step == 3)
      {
        memcpy(buffer, ddata, sizeof(INA3221Data));
        return sizeof(INA3221Data);
      }
    }
  }
  return 0;
}
