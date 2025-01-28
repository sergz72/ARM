#include <malloc.h>
#include "dev_si5351.h"
#include <devices.h>
#include "dev_dds.h"
#include <si5351.h>
#include <string.h>

static const DdsConfig config = {
  .type = DDS_SI5351,
  .level_meter_type = LEVEL_METER_NONE,
  .max_mv = 3300,
  .max_attenuator = 0,
  .mclk = SI5351_XTAL_FREQ
};

static int si5351_command(unsigned char deviceId, unsigned char cmd, dds_cmd *command, int idx)
{
  switch (cmd)
  {
  case DDS_COMMAND_SET_FREQUENCY:
    return si5351_set_freq(idx, command->set_frequency_command.frequency, -900000000, command->channel, command->set_frequency_command.divider);
  case DDS_COMMAND_SET_MODE:
  case DDS_COMMAND_SET_ATTENUATOR:
    return 0;
  case DDS_COMMAND_ENABLE_OUTPUT:
    return si5351_clock_enable(idx, command->channel, command->enable_command.enable);
  default:
    return 1;
  }
}

void* si5351_initializer(int idx, void **data)
{
  si5351_init();

  si5351_write(idx, SI5351_CRYSTAL_LOAD, SI5351_CRYSTAL_LOAD_8PF);

  dev_dds *cfg = malloc(sizeof(dev_dds));
  if (cfg)
  {
    cfg->deviceId = SI5351_DEVICE_ID;
    cfg->command = si5351_command;
    cfg->cfg.max_frequency = 112500000;
    cfg->cfg.min_frequency = 2500;
    cfg->cfg.channels = 3;
    cfg->cfg.max_vout_mV = 3300;
    cfg->cfg.max_attenuator_value = 0;
    cfg->cfg.out_square_divider_bits = 3;
    cfg->cfg.supported_modes = (1 << DDS_MODE_SQUARE);
    cfg->cfg.accumulator_bits = 0;
    cfg->cfg.mclk_MHz = 0;
  }
  return cfg;
}

int si5351_save_config(int idx, void *buffer)
{
  memcpy(buffer, &config, sizeof(DdsConfig));
  return sizeof(DdsConfig);
}
