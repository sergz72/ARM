#include "dev_ad9833.h"
#include <devices.h>
#include <string.h>
#include "dev_dds.h"

static const DdsConfig config = {
  .type = DDS_AD9833,
  .level_meter_type = LEVEL_METER_NONE,
  .max_mv = 3300,
  .max_attenuator = 40,
  .mclk = 25000000
};

void ad9833_initializer(DeviceObject *o)
{
  dds_initializer(AD9833_DEVICE_ID, o);
}

int ad9833_save_config(DeviceObject *o, void *buffer)
{
  memcpy(buffer, &config, sizeof(DdsConfig));
  return sizeof(DdsConfig);
}
