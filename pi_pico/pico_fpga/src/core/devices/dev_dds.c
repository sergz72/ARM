#include <malloc.h>
#include "dev_dds.h"

#include <devices.h>
#include <string.h>

static unsigned char last_command;
static unsigned char command_buffer[10];
static unsigned char *command_buffer_p;
static int bytes_expected;

void* dds_initializer(unsigned char deviceId, int idx)
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
    cfg->command = dds_command;
  }

  bytes_expected = 0;
  command_buffer_p = command_buffer;

  return cfg;
}

static int exec_command(const dev_dds *config, int idx, unsigned char *buffer)
{
  dds_cmd command;
  int rc = 1;

  command.channel = *buffer++;
  switch (last_command)
  {
    case 'f': // set frequency
    case 'c': // set frequency code
      memcpy(&command.set_frequency_command.frequency, buffer, 8);
      buffer += 8;
      memcpy(&command.set_frequency_command.divider, buffer, 2);
      rc = config->command(config->deviceId, last_command == 'f' ? DDS_COMMAND_SET_FREQUENCY : DDS_COMMAND_SET_FREQUENCY_CODE, &command, idx);
      break;
    case 'm': // set mode
      command.set_mode_command.mode = buffer[1];
      rc = config->command(config->deviceId, DDS_COMMAND_SET_MODE, &command, idx);
      break;
    case 'a': // set attenuator
      command.set_attenuator_command.attenuator_value = buffer[1];
      rc = config->command(config->deviceId, DDS_COMMAND_SET_ATTENUATOR, &command, idx);
      break;
    case 'e': // enable output
      command.enable_command.enable = buffer[1];
      rc = config->command(config->deviceId, DDS_COMMAND_ENABLE_OUTPUT, &command, idx);
      break;
    case 's': // sweep
    case 'd': // sweep codes
      memcpy(&command.sweep_command.frequency, buffer, 8);
      buffer += 8;
      memcpy(&command.sweep_command.step, buffer, 4);
      buffer += 4;
      memcpy(&command.sweep_command.points, buffer, 2);
      buffer += 2;
      memcpy(&command.sweep_command.divider, buffer, 2);
      rc = config->command(config->deviceId, last_command == 's' ? DDS_COMMAND_SWEEP : DDS_COMMAND_SWEEP_CODES, &command, idx);
      break;
    default:
      break;
  }
  buffer[0] = rc ? 'e' : 'k';
  return 1;
}

int dds_message_processor(int idx, void *config, void *data, unsigned char *buffer, int len)
{
  const dev_dds *dconfig = (const dev_dds*)config;

  if (bytes_expected)
  {
    memcpy(command_buffer_p, buffer, len);
    if (len >= bytes_expected)
    {
      command_buffer_p = command_buffer;
      return exec_command(dconfig, idx, command_buffer);
    }
    command_buffer_p += len;
    bytes_expected -= len;
    return 0;
  }
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
      bytes_expected = len >= 17 ? 0 : 17 - len;
      break;
    default:
      buffer[0] = 'e';
      return 1;
  }
  if (!bytes_expected)
    return exec_command(config, idx, buffer);
  return 0;
}
