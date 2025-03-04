#include "dev_meter.h"
#include <stdlib.h>
#include <string.h>
#include "generic_dds.h"
#include "generic_meter.h"

static int meter_get_config(MeterConfig *cfg, DeviceObject *o)
{
  unsigned char command = DEVICE_COMMAND_GET_CONFIGURATION;
  return o->transfer(o, &command, 1, (unsigned char*)cfg, sizeof(MeterConfig));
}

void meter_initializer(DeviceObject *o)
{
  int rc;
  MeterConfig *cfg = malloc(sizeof(MeterConfig));
  if (cfg)
  {
    rc = meter_get_config(cfg, o);
    if (rc)
    {
      free(cfg);
      cfg = NULL;
    }
    else
      init_interrupt_pin(o);
  }
  o->device_config = cfg;
}

int meter_timer_event(DeviceObject *o, int step, unsigned char *buffer)
{
  MeterConfig *cfg = (MeterConfig*)o->device_config;
  if (get_interrupt_pin_level(o))
  {
    unsigned char c = DEVICE_COMMAND_GET_RESULTS;
    unsigned int size = cfg->channels * cfg->resolution;
    if (!o->transfer(o, &c, 1, buffer, size))
      return size;
  }
  return 0;
}

int meter_save_config(DeviceObject *o, void *buffer)
{
  MeterConfig *cfg = (MeterConfig*)o->device_config;
  char *name;
  switch (cfg->measure_type)
  {
    case MEASURE_TYPE_VOLTS: name = "Voltmeter"; break;
    case MEASURE_TYPE_AMPERS: name = "Ampermeter"; break;
    case MEASURE_TYPE_OHMS: name = "Ohmmeter"; break;
    case MEASURE_TYPE_FARADS: name = "Capacitance meter"; break;
    case MEASURE_TYPE_HENRY: name = "Inductance meter"; break;
    case MEASURE_TYPE_GRADS: name = "Thermometer"; break;
    case MEASURE_TYPE_HERZ: name = "Frequency counter"; break;
    case MEASURE_TYPE_WATTS: name = "Wattmeter"; break;
    default: name = "Unknown meter"; break;
  }
  return BuildMeterConfig(buffer, cfg, name);
}
