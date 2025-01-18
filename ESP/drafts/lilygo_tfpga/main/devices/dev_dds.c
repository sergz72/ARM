#include <malloc.h>
#include "dev_dds.h"

static unsigned int calculateFrequencyCode(unsigned int frequency, unsigned int mclk_MHz, unsigned int accumulator_bits)
{
  return ((unsigned long long int)frequency << accumulator_bits) / (mclk_MHz * 1000000);
}

static void updateFrequency(dev_dds* config, int idx)
{
  dds_cmd command;
  dds_channel* c = &config->channel_data[config->current_channel];
  if (c->enabled)
  {
    command.channel = config->current_channel;
    if (config->cfg.accumulator_bits)
    {
      command.set_frequency_code_command.frequency_code = calculateFrequencyCode(c->frequency, config->cfg.mclk_MHz, config->cfg.accumulator_bits);
      command.set_frequency_code_command.divider = c->output_divider_value;
      config->command(config->deviceId, DDS_COMMAND_SET_FREQUENCY_CODE, &command, idx);
    }
    else
    {
      command.set_frequency_command.frequency = c->frequency;
      command.set_frequency_command.divider = c->output_divider_value;
      config->command(config->deviceId, DDS_COMMAND_SET_FREQUENCY, &command, idx);
    }
  }
}

static unsigned short calculateAttenuatorValue(unsigned short voltage, dds_config* cfg)
{
  return (unsigned short)((unsigned int)voltage * cfg->max_attenuator_value / cfg->max_vout_mV);
}

static void updateAttenuator(dev_dds* config, int idx)
{
  dds_cmd command;
  dds_channel* c = &config->channel_data[config->current_channel];
  if (c->enabled)
  {
    command.channel = config->current_channel;
    command.set_attenuator_command.attenuator_value = calculateAttenuatorValue(c->output_voltage, &config->cfg);
    config->command(config->deviceId, DDS_COMMAND_SET_ATTENUATOR, &command, idx);
  }
}

void dds_init_channel_data(dev_dds *ddds)
{
  int i;

  ddds->channel_data = malloc(sizeof(dds_channel) * ddds->cfg.channels);
  if (ddds->channel_data)
  {
    for (i = 0; i < ddds->cfg.channels; i++)
    {
      ddds->channel_data[i].frequency = ddds->default_frequency;
      ddds->channel_data[i].mode = 0;//todo find_next_mode(0, ddds->cfg.supported_modes);
      ddds->channel_data[i].enabled = 0;
      ddds->channel_data[i].output_voltage = ddds->cfg.max_vout_mV;
      ddds->channel_data[i].output_divider_value = 1;
    }
  }
}

void* dds_initializer(unsigned char deviceId, unsigned int default_frequency, int idx)
{
  int rc;
  dev_dds *cfg = malloc(sizeof(dev_dds));
  if (cfg)
  {
    rc = dds_get_config(&cfg->cfg, deviceId, idx);
    if (rc)
    {
      free(cfg);
      return NULL;
    }
    cfg->deviceId = deviceId;
    cfg->current_channel = 0;
    cfg->command = dds_command;
    cfg->default_frequency = default_frequency;

    dds_init_channel_data(cfg);
  }
  return cfg;
}

int dds_message_processor(int idx, void *config, void *data, unsigned char *buffer, int len)
{
  buffer[0] = 'e';
  return 1;
}
