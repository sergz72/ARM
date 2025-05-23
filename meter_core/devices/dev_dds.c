#include <malloc.h>
#include "dev_dds.h"
#include <string.h>

static unsigned char last_command;
static unsigned char command_buffer[20];
static unsigned char *command_buffer_p;
static int bytes_expected;

static int dds_command(DeviceObject *o, unsigned char cmd, dds_cmd *data)
{
  dds_i2c_command c;
  c.device_command = DEVICE_COMMAND_DDS_COMMAND;
  c.command = cmd;
  c.channel = data->channel;
  switch (cmd)
  {
    case DDS_COMMAND_ENABLE_OUTPUT:
      c.c1.parameter = data->enable_command.enable;
    return o->transfer(o, (unsigned char*)&c, 4, NULL, 0);
    case DDS_COMMAND_SET_ATTENUATOR:
      c.c1.parameter = data->set_attenuator_command.attenuator_value;
    return o->transfer(o, (unsigned char*)&c, 4, NULL, 0);
    case DDS_COMMAND_SET_FREQUENCY:
    case DDS_COMMAND_SET_FREQUENCY_CODE:
      c.c10.freq = data->set_frequency_command.frequency;
    c.c10.div = data->set_frequency_command.divider;
    return o->transfer(o, (unsigned char*)&c, 13, NULL, 0);
    case DDS_COMMAND_SET_MODE:
      c.c1.parameter = data->set_mode_command.mode;
    return o->transfer(o, (unsigned char*)&c, 4, NULL, 0);
    case DDS_COMMAND_SWEEP:
    case DDS_COMMAND_SWEEP_CODES:
      c.c18.freq = data->sweep_command.frequency;
    c.c18.step = data->sweep_command.step;
    c.c18.points = data->sweep_command.points;
    c.c18.div = data->sweep_command.divider;
    return o->transfer(o, (unsigned char*)&c, 21, NULL, 0);
    default:
      return 1;
  }
}

static int dds_get_config(DdsConfig *cfg, DeviceObject *o)
{
  unsigned char command = DEVICE_COMMAND_GET_CONFIGURATION;
  return o->transfer(o, &command, 1, (unsigned char*)cfg, sizeof(DdsConfig));
}

void dds_initializer(DeviceObject *o)
{
  int rc;
  dev_dds *cfg = malloc(sizeof(dev_dds));
  if (cfg)
  {
    rc = dds_get_config(&cfg->cfg, o);
    if (rc)
    {
      free(cfg);
      cfg = NULL;
    }
    else
    {
      cfg->deviceId = o->device->device_id;
      cfg->command = dds_command;
      if (cfg->cfg.level_meter_type != LEVEL_METER_NONE)
        init_interrupt_pin(o);
      cfg->sweep_points = 0;
    }
  }
  o->device_config = cfg;

  bytes_expected = 0;
  command_buffer_p = command_buffer;
}

int dds_save_config(DeviceObject *o, void *buffer)
{
  memcpy(buffer, &((dev_dds*)o->device_config)->cfg, sizeof(DdsConfig));
  return sizeof(DdsConfig);
}

static int exec_command(dev_dds *config, DeviceObject *o, unsigned char *buffer)
{
  dds_cmd command;
  int rc = 1;

  command_buffer_p = command_buffer;

  command.channel = *command_buffer_p++;
  switch (last_command)
  {
    case 'f': // set frequency
    case 'c': // set frequency code
      config->sweep_points = 0;
      memcpy(&command.set_frequency_command.frequency, command_buffer_p, 8);
      command_buffer_p += 8;
      memcpy(&command.set_frequency_command.divider, command_buffer_p, 2);
      rc = config->command(o, last_command == 'f' ? DDS_COMMAND_SET_FREQUENCY : DDS_COMMAND_SET_FREQUENCY_CODE,
                            &command);
      break;
    case 'm': // set mode
      command.set_mode_command.mode = *command_buffer_p;
      rc = config->command(o, DDS_COMMAND_SET_MODE, &command);
      break;
    case 'a': // set attenuator
      command.set_attenuator_command.attenuator_value = *command_buffer_p;
      rc = config->command(o, DDS_COMMAND_SET_ATTENUATOR, &command);
      break;
    case 'e': // enable output
      command.enable_command.enable = *command_buffer_p;
      if (!command.enable_command.enable)
        config->sweep_points = 0;
      rc = config->command(o, DDS_COMMAND_ENABLE_OUTPUT, &command);
      break;
    case 's': // sweep
    case 'd': // sweep codes
      memcpy(&command.sweep_command.frequency, command_buffer_p, 8);
      command_buffer_p += 8;
      memcpy(&command.sweep_command.step, command_buffer_p, 4);
      command_buffer_p += 4;
      memcpy(&command.sweep_command.points, command_buffer_p, 2);
      command_buffer_p += 2;
      memcpy(&command.sweep_command.divider, command_buffer_p, 2);
      config->sweep_points = command.sweep_command.points;
      rc = config->command(o, last_command == 's' ? DDS_COMMAND_SWEEP : DDS_COMMAND_SWEEP_CODES,
                            &command);
      break;
    default:
      break;
  }
  if (rc)
  {
    buffer[0] = 'e';
    buffer[1] = rc + '0';
    return 2;
  }
  buffer[0] = 'k';
  return 1;
}

int dds_message_processor(DeviceObject *o, unsigned char *buffer, int len)
{
  dev_dds *dconfig = (dev_dds*)o->device_config;

  if (bytes_expected)
  {
    memcpy(command_buffer_p, buffer, len);
    if (len >= bytes_expected)
    {
      bytes_expected = 0;
      return exec_command(dconfig, o, buffer);
    }
    command_buffer_p += len;
    bytes_expected -= len;
    return 0;
  }
  command_buffer_p = command_buffer;
  last_command = buffer[0];
  len--;
  memcpy(command_buffer_p, buffer + 1, len);
  command_buffer_p += len;
  switch (last_command)
  {
    case 'f': // set frequency
    case 'c': // set frequency code
      bytes_expected = len >= 11 ? 0 : 11 - len;
      break;
    case 'm': // set mode
    case 'a': // set attenuator
    case 'e': // enable output
      bytes_expected = len >= 2 ? 0 : 2 - len;
      break;
    case 's': // sweep
    case 'd': // sweep codes
      bytes_expected = len >= 17 ? 0 : 17 - len;
      break;
    default:
      buffer[0] = 'e';
      buffer[1] = '0';
      return 2;
  }
  if (!bytes_expected)
    return exec_command(dconfig, o, buffer);
  return 0;
}

int dds_timer_event(DeviceObject *o, int step, unsigned char *buffer)
{
  dev_dds *cfg = (dev_dds*)o->device_config;
  if (cfg->cfg.level_meter_type != LEVEL_METER_NONE && cfg->sweep_points > 0 && get_interrupt_pin_level(o))
  {
    unsigned char c = DEVICE_COMMAND_GET_RESULTS;
    unsigned int size = cfg->sweep_points * 2;
    if (!o->transfer(o, &c, 1, buffer, size))
      return size;
  }
  return 0;
}
