#include "board.h"
#include "dev_internal_frequency_meter.h"
#include <devices.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  unsigned int frequency1;
  unsigned int frequency2;
  unsigned int frequency1_missing;
  unsigned int frequency2_missing;
} frequency_meter_data;

static const MeterConfig config =
{
  .channels = 2,
  .measure_type = MEASURE_TYPE_HERZ,
  .numbers_before_point = 8,
  .resolution = 4,
  .value_type = VALUE_TYPE_ONES
};

void internal_frequency_meter_initializer(struct _DeviceObject *o)
{
  if (alloc_pio(o->idx))
  {
    o->device_config = NULL;
    return;
  }
  frequency_meter_data *fdata = malloc(sizeof(frequency_meter_data));
  if (fdata)
  {
    fdata->frequency1 = 0;
    fdata->frequency2 = 0;
    fdata->frequency1_missing = 0;
    fdata->frequency2_missing = 0;
    counter_pio_init(o->idx, 0);
    counter_pio_init(o->idx, 1);
  }
  o->device_config = fdata;
}

int internal_frequency_meter_timer_event(struct _DeviceObject *o, int step, unsigned char *buffer)
{
  frequency_meter_data *fdata = (frequency_meter_data*)o->device_config;

  int frequency1 = counter_get(o->idx, 0);
  int frequency2 = counter_get(o->idx, 1);
  int got_f1 = frequency1 >= 0;
  int got_f2 = frequency2 >= 0;

  if (!got_f1)
  {
    fdata->frequency1_missing++;
    if (fdata->frequency1_missing >= 20)
    {
      got_f1 = 1;
      frequency1 = 0;
      fdata->frequency1_missing = 0;
    }
  }
  else
    fdata->frequency1_missing = 0;

  if (!got_f2)
  {
    fdata->frequency2_missing++;
    if (fdata->frequency2_missing >= 20)
    {
      got_f2 = 1;
      frequency2 = 0;
      fdata->frequency2_missing = 0;
    }
  }
  else
    fdata->frequency2_missing = 0;

  if (got_f1 || got_f2)
  {
    if (got_f1)
      fdata->frequency1 = frequency1;
    if (got_f2)
      fdata->frequency2 = frequency2;
    memcpy(buffer, fdata, sizeof(frequency_meter_data));
    return 8;
  }
  return 0;
}

int internal_frequency_meter_save_config(struct _DeviceObject *o, void *buffer)
{
  return BuildMeterConfig(buffer, &config, "Internal frequency meter");
}
