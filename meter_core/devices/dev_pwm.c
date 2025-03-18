#include "dev_pwm.h"
#include <stdlib.h>
#include <string.h>
#include "generic_dds.h"
#include <generic_pwm.h>

static unsigned char last_command;
static unsigned char command_buffer[20];
static unsigned char *command_buffer_p;
static int bytes_expected;

void pwm_initializer(void)
{
  bytes_expected = 0;
  command_buffer_p = command_buffer;
}

static int pwm_get_config(PWMConfig *cfg, DeviceObject *o)
{
  unsigned char command = DEVICE_COMMAND_GET_CONFIGURATION;
  return o->transfer(o, &command, 1, (unsigned char*)cfg, sizeof(PWMConfig));
}

static int exec_command(DeviceObject *o, unsigned char *buffer)
{
  dev_pwm *dev = (dev_pwm *)o->device_config;

  command_buffer_p = command_buffer;

  int rc = 1;
  int channel = *command_buffer_p++;
  unsigned int frequency, duty;
  unsigned short prescaler;

  switch (last_command)
  {
    case 'f': // set frequency
      command_buffer_p += 4;
      memcpy(&frequency, command_buffer_p, 4);
      command_buffer_p += 4;
      memcpy(&duty, command_buffer_p, 4);
      command_buffer_p += 2;
      memcpy(&prescaler, command_buffer_p, 2);
      rc = dev->set_frequency_and_duty(o, channel, prescaler, frequency, duty) ? 3 : 0;
      break;
    case 'e': // enable output
      rc = dev->enable_output(o, channel, *command_buffer_p) ? 2 : 0;
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
  if (bytes_expected)
  {
    memcpy(command_buffer_p, buffer, len);
    if (len >= bytes_expected)
    {
      bytes_expected = 0;
      return exec_command(o, buffer);
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
    return exec_command(o, buffer);
  return 0;
}

static int external_pwm_set_frequency_and_duty(DeviceObject *o, int channel, unsigned short prescaler,
                                                unsigned int frequency, unsigned int duty)
{
  pwm_i2c_command cmd;
  cmd.device_command = DEVICE_COMMAND_PWM_COMMAND;
  cmd.command = PWM_COMMAND_SET_PERIOD_AND_DUTY;
  cmd.channel = channel;
  cmd.c10.prescaler = prescaler;
  cmd.c10.period = frequency;
  cmd.c10.duty = duty;
  return o->transfer(o, (unsigned char*)&cmd, 13, NULL, 0);
}

static int external_pwm_enable_output(DeviceObject *o, int channel, int enable)
{
  pwm_i2c_command cmd;
  cmd.device_command = DEVICE_COMMAND_PWM_COMMAND;
  cmd.command = PWM_COMMAND_ENABLE_OUTPUT;
  cmd.channel = channel;
  cmd.c1.parameter = enable;
  return o->transfer(o, (unsigned char*)&cmd, 4, NULL, 0);;
}

void external_pwm_initializer(DeviceObject *o)
{
  int rc;
  dev_pwm *cfg = malloc(sizeof(dev_pwm));
  if (cfg)
  {
    rc = pwm_get_config(&cfg->cfg, o);
    if (rc)
    {
      free(cfg);
      cfg = NULL;
    }
    else
    {
      cfg->enable_output = external_pwm_enable_output;
      cfg->set_frequency_and_duty = external_pwm_set_frequency_and_duty;
    }
  }
  o->device_config = cfg;
}

int pwm_save_config(DeviceObject *o, void *buffer)
{
  return BuildPWMConfig(buffer, &((dev_pwm*)o->device_config)->cfg, "External PWM");
}
