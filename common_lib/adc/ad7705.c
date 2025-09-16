#include "board.h"
#include <ad7705.h>
#include <string.h>

static unsigned char selected_channel[AD7705_MAX_CHANNELS];
static unsigned char setup_data[AD7705_MAX_CHANNELS];

void ad7705_init(void)
{
  for (int channel = 0; channel < AD7705_MAX_CHANNELS; channel++)
    selected_channel[channel] = 0;
}

void ad7705_write_register(int channel, unsigned char reg_no, const unsigned char *data, int length)
{
  unsigned char l = length + 1;
  unsigned char buffer[l];
  buffer[0] = ((reg_no & 7) << 4) | selected_channel[channel];
  memcpy(buffer+1, data, length);
  ad7705_spi_transfer(channel, buffer, l, NULL, 0);
}

void ad7705_set_channel(int channel, unsigned char channel_no)
{
  channel_no &= 3;
  selected_channel[channel] = channel_no;
  ad7705_spi_transfer(channel, &channel_no, 1, NULL, 0);
}

void ad7705_read_register(int channel, unsigned char reg_no, unsigned char *data, int length)
{
  reg_no = ((reg_no & 7) << 4) | 8 | selected_channel[channel];
  ad7705_spi_transfer(channel, &reg_no, 1, data, length);
}

void ad7705_setup_configuration(int channel, const ad7705_configuration *config)
{
  unsigned char data = config->pga_gain << 3;
  if (config->unipolar)
    data |= 4;
  if (config->buf)
    data |= 2;
  if (config->fsync)
    data |= 1;
  ad7705_write_register(channel, AD7705_REGISTER_SETUP, &data, 1);
  setup_data[channel] = data;
  data = config->filter & 3;
  if (config->clkdis)
    data |= 0x10;
  if (config->clkdiv)
    data |= 8;
  if (config->clk)
    data |= 4;
  ad7705_write_register(channel, AD7705_REGISTER_CLOCK, &data, 1);
}

int ad7705_wait(int channel, int timeout)
{
  while (timeout)
  {
    delayms(1);
    if (!AD7705_DRDY_GET(channel))
      break;
    timeout--;
  }
  return timeout == 0;
}

void ad7705_start_conversion(int channel, int channel_no)
{
  ad7705_set_channel(channel, channel_no);
  unsigned char data = setup_data[channel] & 0xFE;
  ad7705_write_register(channel, AD7705_REGISTER_SETUP, &data, 1);
}

void ad7705_stop_conversion(int channel)
{
  unsigned char data = setup_data[channel] | 1;
  ad7705_write_register(channel, AD7705_REGISTER_SETUP, &data, 1);
}

int ad7705_read(int channel, unsigned char channel_no, int *result, int timeout)
{
  ad7705_start_conversion(channel, channel_no);
  int rc = ad7705_wait(channel, timeout);
  if (rc)
    return rc;
  ad7705_stop_conversion(channel);
  unsigned char data[2];
  ad7705_read_register(channel, AD7705_REGISTER_DATA, data, 2);
  int value = (data[0] << 8) | data[1];
  if (!(setup_data[channel] & 4)) // bipolar
    value -= 0x8000;
  *result = value;
  return 0;
}

int ad7705_read_voltage(int channel, unsigned char channel_no, double input_divider_coefficient, double vref,
                        ad7705_data *result, int timeout)
{
  int rc = ad7705_read(channel, channel_no, &result->value, timeout);
  if (rc)
    return rc;
  double full_scale;
  if (setup_data[channel] & 4) // unipolar
    full_scale = 0xFFFF;
  else
    full_scale = 0x7FFF;
  result->voltage = (double)result->value * vref / full_scale / input_divider_coefficient;
  return 0;
}

int ad7705_get_offset(int channel, unsigned char channel_no)
{
  ad7705_set_channel(channel, channel_no);
  unsigned char data[3];
  ad7705_read_register(channel, AD7705_REGISTER_OFFSET, data, 3);
  return (data[0] << 16) | (data[1] << 8) | data[2];
}

void ad7705_set_offset(int channel, unsigned char channel_no, int value)
{
  ad7705_set_channel(channel, channel_no);
  unsigned char data[3];
  data[0] = (unsigned char)(value >> 16);
  data[1] = (unsigned char)(value >> 8);
  data[2] = (unsigned char)value;
  ad7705_write_register(channel, AD7705_REGISTER_OFFSET, data, 3);
}

int ad7705_get_gain(int channel, unsigned char channel_no)
{
  ad7705_set_channel(channel, channel_no);
  unsigned char data[3];
  ad7705_read_register(channel, AD7705_REGISTER_GAIN, data, 3);
  return (data[0] << 16) | (data[1] << 8) | data[2];
}

void ad7705_set_gain(int channel, unsigned char channel_no, int value)
{
  ad7705_set_channel(channel, channel_no);
  unsigned char data[3];
  data[0] = (unsigned char)(value >> 16);
  data[1] = (unsigned char)(value >> 8);
  data[2] = (unsigned char)value;
  ad7705_write_register(channel, AD7705_REGISTER_GAIN, data, 3);
}

unsigned char ad7705_get_pga(int channel)
{
  return (setup_data[channel] >> 3) & 7;
}

void ad7705_set_pga(int channel, unsigned char value)
{
  setup_data[channel] = (setup_data[channel] & 0xC7) | ((value & 7) << 3);
  ad7705_write_register(channel, AD7705_REGISTER_SETUP, &setup_data[channel], 1);
}

int ad7705_calibration(int channel, unsigned char channel_no, unsigned char mode, int timeout)
{
  unsigned char data = (setup_data[channel] | mode) & 0xFE;
  ad7705_write_register(channel, AD7705_REGISTER_SETUP, &data, 1);
  int rc = ad7705_wait(channel, timeout);
  ad7705_stop_conversion(channel);
  return rc;
}
