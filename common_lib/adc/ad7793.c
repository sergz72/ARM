#include "board.h"
#include <ad7793.h>
#include <string.h>

static unsigned char configurations[AD7793_MAX_CHANNELS][2];
static unsigned char update_rates[AD7793_MAX_CHANNELS];

void ad7793_init(void)
{
  for (int channel = 0; channel < AD7793_MAX_CHANNELS; channel++)
  {
    configurations[channel][0] = 7;
    configurations[channel][1] = 0x10;
    update_rates[channel] = 0x0A;
  }
}

void ad7793_write_register(int channel, unsigned char reg_no, const unsigned char *data, int length)
{
  unsigned char l = length + 1;
  unsigned char buffer[l];
  buffer[0] = (reg_no & 7) << 3;
  memcpy(buffer+1, data, length);
  ad7793_spi_transfer(channel, buffer, l, NULL, 0);
}

void ad7793_set_mode(int channel, unsigned char mode)
{
  unsigned char data[2];
  data[0] = mode & 0xE0;
  data[1] = update_rates[channel];
  ad7793_write_register(channel, AD7793_REGISTER_MODE, data, 2);
}

void ad7793_set_mode_and_update_rate(int channel, unsigned char mode, unsigned char update_rate)
{
  unsigned char data[2];
  data[0] = mode & 0xE0;
  data[1] = update_rate & 0x0F;
  ad7793_write_register(channel, AD7793_REGISTER_MODE, data, 2);
  update_rates[channel] = data[1];
}

void ad7793_read_register(int channel, unsigned char reg_no, unsigned char *data, int length)
{
  reg_no = ((reg_no & 7) << 3) | 0x40;
  ad7793_spi_transfer(channel, &reg_no, 1, data, length);
}

void ad7793_set_configuration(int channel, const ad7793_configuration *config)
{
  unsigned char data[2];
  data[0] = (config->vbias << 6) | config->gain;
  if (config->burnout_enable)
    data[0] |= 0x20;
  if (config->unipolar)
    data[0] |= 0x10;
  if (config->boost)
    data[0] |= 8;
  data[1] = config->channel;
  if (config->internal_reference)
    data[1] |= 0x80;
  if (config->buffer_enable)
    data[1] |= 0x10;
  ad7793_write_register(channel, AD7793_REGISTER_CONFIGURATION, data, 2);
  configurations[channel][0] = data[0];
  configurations[channel][1] = data[1];
}

void ad7793_set_channel_and_gain(int channel, unsigned char channel_no, unsigned char gain)
{
  unsigned char data[2];
  data[0] = (configurations[channel][0] & 0xF8) | (gain & 7);
  data[1] = (configurations[channel][1] & 0xF8) | (channel_no & 7);
  ad7793_write_register(channel, AD7793_REGISTER_CONFIGURATION, data, 2);
  configurations[channel][0] = data[0];
  configurations[channel][1] = data[1];
}

void ad7793_set_bias(int channel, unsigned char bias)
{
  unsigned char data[2];
  data[0] = (configurations[channel][0] & 0x3F) | (bias << 6);
  data[1] = configurations[channel][1];
  ad7793_write_register(channel, AD7793_REGISTER_CONFIGURATION, data, 2);
  configurations[channel][0] = data[0];
}

void ad7793_set_io(int channel, unsigned char iexcdir, unsigned char iexcen)
{
  iexcdir = ((iexcdir << 2) & 0x0C) | (iexcen & 3);
  ad7793_write_register(channel, AD7793_REGISTER_IO, &iexcdir, 1);
}

int ad7793_wait(int channel, int timeout)
{
  while (timeout)
  {
    delayms(1);
    if (!AD7793_RDY_GET(channel))
      break;
    timeout--;
  }
  return timeout == 0;
}

int ad7793_calibrate_offset_internal(int channel, unsigned char channel_no, unsigned char gain, int timeout)
{
  ad7793_set_channel_and_gain(channel, channel_no, gain);
  ad7793_set_mode(channel, AD7793_MODE_INTERNAL_OFFSET_CALIBRATION);
  return ad7793_wait(channel, timeout);
}

int ad7793_calibrate_gain_internal(int channel, unsigned char channel_no, unsigned char gain, int timeout)
{
  ad7793_set_channel_and_gain(channel, channel_no, gain);
  ad7793_set_mode(channel, AD7793_MODE_INTERNAL_GAIN_CALIBRATION);
  return ad7793_wait(channel, timeout);
}

int ad7793_calibrate_offset_system(int channel, unsigned char channel_no, unsigned char gain, int timeout)
{
  ad7793_set_channel_and_gain(channel, channel_no, gain);
  ad7793_set_mode(channel, AD7793_MODE_SYSTEM_OFFSET_CALIBRATION);
  return ad7793_wait(channel, timeout);
}

int ad7793_calibrate_gain_system(int channel, unsigned char channel_no, unsigned char gain, int timeout)
{
  ad7793_set_channel_and_gain(channel, channel_no, gain);
  ad7793_set_mode(channel, AD7793_MODE_SYSTEM_GAIN_CALIBRATION);
  return ad7793_wait(channel, timeout);
}

int ad7793_get_offset(int channel, unsigned char channel_no)
{
  int offset;
  unsigned char data[3];
  ad7793_set_channel_and_gain(channel, channel_no, AD7793_GAIN_1);
  ad7793_read_register(channel, AD7793_REGISTER_OFFSET, data, 3);
  offset = (data[0] << 16) | (data[1] << 8) | data[2];
  return offset;
}

void ad7793_set_offset(int channel, unsigned char channel_no, int value)
{
  unsigned char data[3];
  data[0] = (unsigned char)(value >> 16);
  data[1] = (unsigned char)(value >> 8);
  data[2] = (unsigned char)value;
  ad7793_set_channel_and_gain(channel, channel_no, AD7793_GAIN_1);
  ad7793_write_register(channel, AD7793_REGISTER_OFFSET, data, 3);
}

int ad7793_get_gain(int channel, unsigned char channel_no)
{
  int gain;
  unsigned char data[3];
  ad7793_set_channel_and_gain(channel, channel_no, AD7793_GAIN_1);
  ad7793_read_register(channel, AD7793_REGISTER_GAIN, data, 3);
  gain = (data[0] << 16) | (data[1] << 8) | data[2];
  return gain;
}

void ad7793_set_gain(int channel, unsigned char channel_no, int value)
{
  unsigned char data[3];
  data[0] = (unsigned char)(value >> 16);
  data[1] = (unsigned char)(value >> 8);
  data[2] = (unsigned char)value;
  ad7793_set_channel_and_gain(channel, channel_no, AD7793_GAIN_1);
  ad7793_write_register(channel, AD7793_REGISTER_GAIN, data, 3);
}

int ad7793_read(int channel, unsigned char channel_no, unsigned char gain, int *result, int timeout)
{
  unsigned char data[3];
  if (!AD7793_RDY_GET(channel))
    ad7793_read_register(channel, AD7793_REGISTER_DATA, data, 3);
  ad7793_set_mode(channel, AD7793_MODE_SINGLE_CONVERSION);
  int rc = ad7793_wait(channel, timeout);
  if (rc)
    return rc;
  ad7793_read_register(channel, AD7793_REGISTER_DATA, data, 3);
  *result = (data[0] << 16) | (data[1] << 8) | data[2];
  return 0;
}

int ad7793_read_voltage(const ad7793_read_voltage_configuration *configuration,
                        ad7793_data *result, int timeout)
{
  int rc = ad7793_read(configuration->channel, configuration->channel_no, configuration->gain, &result->value, timeout);
  if (rc)
    return rc;
  if (configurations[configuration->channel][0] & 0x10) // unipolar
    result->voltage = (double)result->value * configuration->vref / (double)0xFFFFFF / configuration->input_divider_coefficient;
  else
    result->voltage = (double)(result->value - 0x800000) * configuration->vref / (double)0x7FFFFF / configuration->input_divider_coefficient;
  return 0;
}
