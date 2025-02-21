#include "dev_pwm.h"
#include <string.h>

static unsigned char last_command;
static unsigned char command_buffer[20];
static unsigned char *command_buffer_p;
static int bytes_expected;

void pwm_initializer(void)
{
  bytes_expected = 0;
  command_buffer_p = command_buffer;
}

static int exec_command(dev_pwm *dev, void *data, int idx, unsigned char *buffer)
{
  command_buffer_p = command_buffer;

  int rc = 1;
  int channel = *command_buffer_p++;
  unsigned int frequency, duty;

  switch (last_command)
  {
    case 'f': // set frequency
      command_buffer_p += 4;
      memcpy(&frequency, command_buffer_p, 4);
      command_buffer_p += 4;
      memcpy(&duty, command_buffer_p, 4);
      rc = dev->set_frequency_and_duty(idx, dev->cfg, data, channel, frequency, duty) ? 3 : 0;
      break;
    case 'e': // enable output
      rc = dev->enable_output(idx, dev->cfg, data, channel, *command_buffer_p) ? 2 : 0;
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

int pwm_message_processor(DeviceObject *o, unsigned char *buffer, int len)
{
  dev_pwm *dev = (dev_pwm *)o->device_config;

  if (bytes_expected)
  {
    memcpy(command_buffer_p, buffer, len);
    if (len >= bytes_expected)
    {
      bytes_expected = 0;
      return exec_command(dev, o->device_data, o->idx, buffer);
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
      bytes_expected = len >= 13 ? 0 : 13 - len;
      break;
    case 'e': // enable output
      bytes_expected = len >= 2 ? 0 : 2 - len;
      break;
    default:
      buffer[0] = 'e';
      buffer[1] = '0';
      return 2;
  }
  if (!bytes_expected)
    return exec_command(dev, o->device_data, o->idx, buffer);
  return 0;
}
