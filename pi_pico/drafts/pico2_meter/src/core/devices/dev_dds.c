#include <malloc.h>
#include "dev_dds.h"
#include <string.h>

static unsigned char last_command;
static unsigned char command_buffer[20];
static unsigned char *command_buffer_p;
static int bytes_expected;

void dds_initializer(unsigned char deviceId, DeviceObject *o)
{
  int rc;
  dev_dds *cfg = malloc(sizeof(dev_dds));
  if (cfg)
  {
    rc = dds_get_config(&cfg->cfg, deviceId, o->idx);
    if (rc)
    {
      free(cfg);
      cfg = NULL;
    }
    else
    {
      cfg->deviceId = deviceId;
      cfg->command = dds_command;
    }
  }
  o->device_config = cfg;

  bytes_expected = 0;
  command_buffer_p = command_buffer;
}

static int exec_command(const dev_dds *config, int idx, unsigned char *buffer)
{
  dds_cmd command;
  int rc = 1;

  command_buffer_p = command_buffer;

  command.channel = *command_buffer_p++;
  switch (last_command)
  {
    case 'f': // set frequency
    case 'c': // set frequency code
      memcpy(&command.set_frequency_command.frequency, command_buffer_p, 8);
      command_buffer_p += 8;
      memcpy(&command.set_frequency_command.divider, command_buffer_p, 2);
      rc = config->command(config->deviceId,
                            last_command == 'f' ? DDS_COMMAND_SET_FREQUENCY : DDS_COMMAND_SET_FREQUENCY_CODE,
                            &command, idx, config->device_config);
      break;
    case 'm': // set mode
      command.set_mode_command.mode = *command_buffer_p;
      rc = config->command(config->deviceId, DDS_COMMAND_SET_MODE, &command, idx, config->device_config);
      break;
    case 'a': // set attenuator
      command.set_attenuator_command.attenuator_value = *command_buffer_p;
      rc = config->command(config->deviceId, DDS_COMMAND_SET_ATTENUATOR, &command, idx, config->device_config);
      break;
    case 'e': // enable output
      command.enable_command.enable = *command_buffer_p;
      rc = config->command(config->deviceId, DDS_COMMAND_ENABLE_OUTPUT, &command, idx, config->device_config);
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
      rc = config->command(config->deviceId, last_command == 's' ? DDS_COMMAND_SWEEP : DDS_COMMAND_SWEEP_CODES,
                            &command, idx, config->device_config);
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
  const dev_dds *dconfig = (const dev_dds*)o->device_config;

  if (bytes_expected)
  {
    memcpy(command_buffer_p, buffer, len);
    if (len >= bytes_expected)
    {
      bytes_expected = 0;
      return exec_command(dconfig, o->idx, buffer);
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
    return exec_command(dconfig, o->idx, buffer);
  return 0;
}
