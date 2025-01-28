#include "dev_pwm.h"

#include <board.h>
#include <string.h>

static unsigned char last_command;
static unsigned char command_buffer[10];
static unsigned char *command_buffer_p;
static int bytes_expected;

void pwm_initializer(void)
{
  bytes_expected = 0;
  command_buffer_p = command_buffer;
}

static int exec_command(dev_pwm *dev, void *data, int idx, unsigned char *buffer)
{
  int rc = 1;
  int channel = *buffer++;
  unsigned int frequency, duty;

  switch (last_command)
  {
    case 'f': // set frequency
      memcpy(&frequency, buffer, 4);
      buffer += 4;
      memcpy(&duty, buffer, 4);
      rc = dev->set_frequency_and_duty(idx, dev->cfg, data, channel, frequency, duty);
      break;
    case 'e': // enable output
      rc = dev->enable_output(idx, dev->cfg, data, channel, *buffer);
      break;
    default:
      break;
  }
  buffer[0] = rc ? 'e' : 'k';
  return 1;
}

int pwm_message_processor(int idx, void *config, void *data, unsigned char *buffer, int len)
{
  dev_pwm *dev = (dev_pwm *)config;

  if (bytes_expected)
  {
    memcpy(command_buffer_p, buffer, len);
    if (len >= bytes_expected)
    {
      command_buffer_p = command_buffer;
      return exec_command(dev, data, idx, command_buffer);
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
      bytes_expected = len >= 8 ? 0 : 8 - len;
      break;
    case 'e': // enable output
      bytes_expected = len >= 2 ? 0 : 2 - len;
      break;
    default:
      buffer[0] = 'e';
      return 1;
  }
  if (!bytes_expected)
    return exec_command(dev, data, idx, buffer);
  return 0;
}
