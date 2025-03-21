#include <malloc.h>
#include "dev_ads1115.h"
#include <ads1115.h>
#include <string.h>

typedef struct {
  int voltage[4];
} DEV_ADS1115Data;

typedef struct {
  int divider[4];
} DEV_ADS1115Config;

static const ADS1115Config dcfg[4] = {
    {
        .dr = ADS1115_DR_8,
        .mux = ADS1115_MUX_AIN0_GND,
        .pga = ADS1115_PGA_4096,
        .comp_que = ADS1115_COMP_QUE_DISABLE,
        .os = 1,
        .single_shot_mode = 1,
        .comp_active_high = 0,
        .comp_mode_window = 0,
        .comp_latching = 0,
    },
    {
        .dr = ADS1115_DR_8,
        .mux = ADS1115_MUX_AIN1_GND,
        .pga = ADS1115_PGA_4096,
        .comp_que = ADS1115_COMP_QUE_DISABLE,
        .os = 1,
        .single_shot_mode = 1,
        .comp_active_high = 0,
        .comp_mode_window = 0,
        .comp_latching = 0,
    },
    {
        .dr = ADS1115_DR_8,
        .mux = ADS1115_MUX_AIN2_GND,
        .pga = ADS1115_PGA_4096,
        .comp_que = ADS1115_COMP_QUE_DISABLE,
        .os = 1,
        .single_shot_mode = 1,
        .comp_active_high = 0,
        .comp_mode_window = 0,
        .comp_latching = 0,
    },
    {
        .dr = ADS1115_DR_8,
        .mux = ADS1115_MUX_AIN3_GND,
        .pga = ADS1115_PGA_4096,
        .comp_que = ADS1115_COMP_QUE_DISABLE,
        .os = 1,
        .single_shot_mode = 1,
        .comp_active_high = 0,
        .comp_mode_window = 0,
        .comp_latching = 0,
    }
};

static const MeterConfig config = {
  .channels = 4,
  .resolution = 4, // 16 bit
  .measure_type = MEASURE_TYPE_VOLTS,
  .value_type = VALUE_TYPE_MICROS,
  .numbers_before_point = 2
};

void ads1115_initializer(DeviceObject *o)
{
  DEV_ADS1115Config* cfg = malloc(sizeof(DEV_ADS1115Config));
  if (cfg)
  {
    cfg->divider[0] = cfg->divider[1] = cfg->divider[2] = cfg->divider[3] = 10;
    o->device_data = malloc(sizeof(DEV_ADS1115Data));
  }
  o->device_config = cfg;
}

static void getResult(int idx, int channel, int divider, DEV_ADS1115Data *data)
{
  int rc;

  if (data)
  {
    int v;

    rc = ads1115GetVoltage(idx, ADS1115_DEVICE_ID, dcfg[channel].pga, &v);
    if (rc)
      data->voltage[channel] = 0;
    else
    {
      data->voltage[channel] = v * divider / 1000;
    }
  }
}

int ads1115_timer_event(DeviceObject *o, int step, unsigned char *buffer)
{
  DEV_ADS1115Config* cfg = (DEV_ADS1115Config*)o->device_config;
  DEV_ADS1115Data *ddata = (DEV_ADS1115Data*)o->device_data;

  switch (step)
  {
    case 1:
      ads1115SetConfig(o->idx, ADS1115_DEVICE_ID, &dcfg[0]);
      break;
    case 3:
      getResult(o->idx, 0, cfg->divider[0], ddata);
      ads1115SetConfig(o->idx, ADS1115_DEVICE_ID, &dcfg[1]);
      break;
    case 5:
      getResult(o->idx, 1, cfg->divider[1], ddata);
      ads1115SetConfig(o->idx, ADS1115_DEVICE_ID, &dcfg[2]);
      break;
    case 7:
      getResult(o->idx, 2, cfg->divider[2], ddata);
      ads1115SetConfig(o->idx, ADS1115_DEVICE_ID, &dcfg[3]);
      break;
    case 9:
      getResult(o->idx, 3, cfg->divider[3], ddata);
      memcpy(buffer, ddata->voltage, 16);
      return 16;
    default:
      break;
  }
  return 0;
}

int ads1115_save_config(DeviceObject *o, void *buffer)
{
  return BuildMeterConfig(buffer, &config, "ADS1115");
}
