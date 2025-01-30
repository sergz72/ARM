#include "board.h"
#include "dev_internal_frequency_meter.h"

#include <devices.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  unsigned int frequency1;
  unsigned int frequency2;
} frequency_meter_data;

static const MeterConfig config1 =
{
  .channels = 1,
  .measure_type = MEASURE_TYPE_HERZ,
  .numbers_before_point = 8,
  .resolution = 4,
  .value_type = VALUE_TYPE_ONES
};

static const MeterConfig config2 =
{
  .channels = 2,
  .measure_type = MEASURE_TYPE_HERZ,
  .numbers_before_point = 8,
  .resolution = 4,
  .value_type = VALUE_TYPE_ONES
};

void* internal_frequency_meter_initializer(int idx, void **data)
{
  frequency_meter_data *fdata = malloc(sizeof(frequency_meter_data));
  if (fdata != NULL)
  {
    fdata->frequency1 = 0;
    fdata->frequency2 = 0;
    *data = fdata;
    if (idx == 4)
      counter_pio_init(COUNTER4_0_SM, PIN_4_0);
    else
    {
      counter_pio_init(COUNTER5_0_SM, PIN_5_0);
      counter_pio_init(COUNTER5_1_SM, PIN_5_1);
    }
  }
  return (void*)1;
}

int internal_frequency_meter_timer_event(int idx, int step, void* config, void *data, int interrupt, unsigned char *buffer)
{
  frequency_meter_data *fdata = (frequency_meter_data*)data;

  int frequency1 = counter_get(idx, 0);
  int frequency2 = idx == 4 ? -1 : counter_get(idx, 1);
  int got_f1 = frequency1 >= 0;
  int got_f2 = frequency2 >= 0;
  if (got_f1 || got_f2)
  {
    if (got_f1)
      fdata->frequency1 = frequency1;
    if (got_f2)
      fdata->frequency2 = frequency2;
    memcpy(buffer, fdata, sizeof(frequency_meter_data));
    return idx == 4 ? 4 : 8;
  }
  return 0;
}

int internal_frequency_meter_save_config(int idx, void *buffer)
{
  return BuildMeterConfig(buffer, idx == 4 ? &config1 : &config2, "Internal frequency meter");
}
