#include <malloc.h>
#include "dev_si5351.h"
#include "dev_dds.h"
#include <si5351.h>
#include <string.h>

static const DdsConfig config =
{
  .type = DDS_SI5351,
  .level_meter_type = LEVEL_METER_NONE,
  .max_mv = 3300,
  .max_attenuator = 0,
  .mclk = SI5351_XTAL_FREQ
};

static int si5351_command(DeviceObject *o, unsigned char cmd, dds_cmd *command)
{
  if (o->device_config == NULL)
    return 9;
  switch (cmd)
  {
  case DDS_COMMAND_SET_FREQUENCY:
    return si5351_set_frequency((si5351_dev*)o->device_config, command->channel, command->set_frequency_command.frequency / 10,
                                 command->set_frequency_command.divider);
  case DDS_COMMAND_SET_MODE:
  case DDS_COMMAND_SET_ATTENUATOR:
    return 0;
  case DDS_COMMAND_ENABLE_OUTPUT:
    return si5351_enable_output((si5351_dev*)o->device_config, command->channel, command->enable_command.enable);
  default:
    return 1;
  }
}

void si5351_initializer(DeviceObject *o)
{
  si5351_conf device_config;
  si5351_dev *device = malloc(sizeof(si5351_dev));
  if (device == NULL)
  {
    o->device_config = NULL;
    return;
  }

  device_config.channel = o->idx;
  device_config.device_address = SI5351_DEVICE_ID;
  device_config.load = SI5351_CRYSTAL_LOAD_8PF;
  device_config.output_count = 3;
  device_config.variant = SI5351_VARIANT_A;
  device_config.mClk = SI5351_XTAL_FREQ;

  if (si5351_init(&device_config, device))
  {
    free(device);
    o->device_config = NULL;
    return;
  }

  dev_dds *cfg = malloc(sizeof(dev_dds));
  if (cfg)
  {
    memcpy(&cfg->cfg, &config, sizeof(DdsConfig));
    cfg->deviceId = SI5351_DEVICE_ID;
    cfg->command = si5351_command;
    cfg->device_config = device;
  }
  o->device_config = cfg;
}

int si5351_save_config(DeviceObject *o, void *buffer)
{
  memcpy(buffer, &config, sizeof(DdsConfig));
  return sizeof(DdsConfig);
}
