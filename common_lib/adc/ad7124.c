#include "board.h"
#include <ad7124.h>
#include <string.h>

static unsigned short adc_control[AD7124_MAX_CHANNELS];
static unsigned short channel_config[AD7124_MAX_CHANNELS][8];

void ad7124_init(void)
{
  for (int channel = 0; channel < AD7124_MAX_CHANNELS; channel++)
    adc_control[channel] = 0;
}

void ad7124_read_register(int channel, unsigned char reg_no, unsigned char *data, int length)
{
  reg_no |= 0x40; // read operation
  ad7124_spi_transfer(channel, &reg_no, 1, data, length);
}

void ad7124_write_register(int channel, unsigned char reg_no, const unsigned char *data, int length)
{
  unsigned char l = length + 1;
  unsigned char buffer[l];
  buffer[0] = reg_no;
  memcpy(buffer+1, data, length);
  ad7124_spi_transfer(channel, buffer, l, NULL, 0);
}

void ad7124_setup_channel(int channel, unsigned char channel_no, const ad7124_channel_configuration *config)
{
  unsigned char data[2];

  data[0] = config->enable ? 0x80 : 0;
  data[0] |= config->setup_no << 4;
  data[0] |= (config->ainp >> 3) & 3;
  data[1] = (config->ainp << 5) | config->ainm;
  ad7124_write_register(channel, AD7124_REGISTER_CHANNEL0 + channel_no, data, 2);
  channel_config[channel][channel_no] = (data[0] << 8) | data[1];
}

void ad7124_setup_configuration(int channel, unsigned char config_no, const ad7124_configuration *config)
{
  unsigned char data[3];

  data[0] = config->bipolar ? 8 : 0;
  data[0] |= config->burnout << 1;
  if (config->ref_bufp)
    data[0] |= 1;
  data[1] = config->ref_bufm ? 0x80 : 0;
  if (config->ain_bufp)
    data[1] |= 0x40;
  if (config->ain_bufm)
    data[1] |= 0x20;
  data[1] |= (config->refsel << 3) | config->pga;
  ad7124_write_register(channel, AD7124_REGISTER_CONFIG0 + config_no, data, 2);
  data[0] = config->filter_type << 5;
  if (config->filter_rej60)
    data[0] |= 0x10;
  if (config->filter_single_cycle)
    data[0] |= 1;
  data[0] |= config->post_filter << 1;
  data[1] = (config->filter_fs >> 8) & 7;
  data[2] = (unsigned char)config->filter_fs;
  ad7124_write_register(channel, AD7124_REGISTER_FILTER0 + config_no, data, 2);
}

void ad7124_set_mode(int channel, unsigned char mode)
{
  unsigned short value = (adc_control[channel] & 0xFFC3) | mode;
  unsigned char data[2];
  data[0] = (unsigned char)(value >> 8);
  data[1] = (unsigned char)value;
  ad7124_write_register(channel, AD7124_REGISTER_ADC_CONTROL, data, 2);
}

int ad7124_wait(int channel, int timeout)
{
  while (timeout)
  {
    delayms(1);
    if (!AD7124_RDY_GET(channel))
      break;
    timeout--;
  }
  return timeout == 0;
}

void ad7124_enable_channel(int channel, int channel_no)
{
  unsigned char data[2];
  data[0] = (unsigned char)((channel_config[channel][channel_no] >> 8) | 0x80);
  data[1] = (unsigned char)channel_config[channel][channel_no];
  ad7124_write_register(channel, AD7124_REGISTER_CHANNEL0 + channel_no, data, 2);
}

void ad7124_disable_channel(int channel, int channel_no)
{
  unsigned char data[2];
  data[0] = (unsigned char)((channel_config[channel][channel_no] >> 8) & 0x7F);
  data[1] = (unsigned char)channel_config[channel][channel_no];
  ad7124_write_register(channel, AD7124_REGISTER_CHANNEL0 + channel_no, data, 2);
}

int ad7124_read(int channel, ad7124_data *result, unsigned int channels, int timeout)
{
  int channel_no = 0;
  while (channels)
  {
    if (channels & 1)
    {
      ad7124_enable_channel(channel, channel_no);
      ad7124_set_mode(channel, AD7124_MODE_SINGLE_CONVERSION);
      int rc = ad7124_wait(channel, timeout);
      if (rc)
        return rc;
      ad7124_disable_channel(channel, channel_no);
      unsigned char data[3];
      ad7124_read_register(channel, AD7124_REGISTER_DATA, data, 3);
      result->value = (data[0] << 16) | (data[1] << 8) | data[2];
      result->channel = channel_no;
      result++;
    }
    channel_no++;
    channels >>= 1;
  }
  return 0;
}

int ad7124_read_voltage(int channel, unsigned int bipolar_channels, double input_divider_coefficient, double vref,
                        ad7124_data *result, unsigned int channels, int timeout)
{
  int rc = ad7124_read(channel, result, channels, timeout);
  if (rc)
    return rc;
  while (channels)
  {
    if (channels & 1)
    {
      if (bipolar_channels & (1<<result->channel))
        result->voltage = (double)(result->value - 0x800000) * vref / (double)0x7FFFFF / input_divider_coefficient;
      else
        result->voltage = (double)result->value * vref / (double)0xFFFFFF / input_divider_coefficient;
      result++;
    }
    channels >>= 1;
  }
  return 0;
}

int ad7124_calibrate_offset_internal(int channel, unsigned char channel_no, int timeout)
{
  ad7124_enable_channel(channel, channel_no);
  ad7124_set_mode(channel, AD7124_MODE_INTERNAL_OFFSET_CALIBRATION);
  int rc = ad7124_wait(channel, timeout);
  ad7124_disable_channel(channel, channel_no);
  return rc;
}

int ad7124_calibrate_gain_internal(int channel, unsigned char channel_no, int timeout)
{
  ad7124_enable_channel(channel, channel_no);
  ad7124_set_mode(channel, AD7124_MODE_INTERNAL_GAIN_CALIBRATION);
  int rc = ad7124_wait(channel, timeout);
  ad7124_disable_channel(channel, channel_no);
  return rc;
}

int ad7124_calibrate_offset_system(int channel, unsigned char channel_no, int timeout)
{
  ad7124_enable_channel(channel, channel_no);
  ad7124_set_mode(channel, AD7124_MODE_SYSTEM_OFFSET_CALIBRATION);
  int rc = ad7124_wait(channel, timeout);
  ad7124_disable_channel(channel, channel_no);
  return rc;
}

int ad7124_calibrate_gain_system(int channel, unsigned char channel_no, int timeout)
{
  ad7124_enable_channel(channel, channel_no);
  ad7124_set_mode(channel, AD7124_MODE_SYSTEM_GAIN_CALIBRATION);
  int rc = ad7124_wait(channel, timeout);
  ad7124_disable_channel(channel, channel_no);
  return rc;
}

int ad7124_calibrate_gain(ad7124_gain_calibration *calibration, int timeout)
{
  //todo
  return 1;
}

int ad7124_calibrate_offset(int channel, unsigned char channel_no, unsigned char config_no, int num_loops, int timeout)
{
  //todo
  return 1;
}

void ad7124_setup_adc_control(int channel, const ad7124_adc_control *config)
{
  unsigned char data[2];
  data[0] = config->dout_rdy_del ? 0x10 : 0;
  if (config->cont_read)
    data[0] |= 8;
  if (config->data_status)
    data[0] |= 4;
  if (config->cs_en)
    data[0] |= 2;
  if (config->ref_en)
    data[0] |= 1;
  data[1] = config ->clk_sel | config->mode | (config->power_mode << 6);
  ad7124_write_register(channel, AD7124_REGISTER_ADC_CONTROL, data, 2);
  adc_control[channel] = (data[0] << 8) | data[1];
}

void ad7124_setup_io_control(int channel, const ad7124_io_control *config)
{
  unsigned char data[3];
  data[0] = (config->gpio_dat << 4) | config->gpio_ctrl;
  data[1] = config->iout0 | (config->iout1 << 4) | (config->pdsw ? 0x80 : 0);
  data[2] = config->iout0_ch | (config->iout1_ch << 4);
  ad7124_write_register(channel, AD7124_REGISTER_IO_CONTROL1, data, 3);
  data[0] = (unsigned char)(config->vbias >> 8);
  data[1] = (unsigned char)config->vbias;
  ad7124_write_register(channel, AD7124_REGISTER_IO_CONTROL2, data, 2);
}

void ad7124_setup_error_control(int channel, const ad7124_error_control *config)
{
  unsigned char data[3];
  data[0] = config->mclk_cnt_en ? 0x40 : 0;
  if (config->ldo_cap_chk_test_en)
    data[0] |= 0x20;
  data[0] |= config->ldo_cap_chk << 3;
  if (config->adc_cal_err_en)
    data[0] |= 4;
  if (config->adc_conv_err_en)
    data[0] |= 2;
  if (config->adc_sat_err_en)
    data[0] |= 1;
  data[1] = config->ainp_ov_err_en ? 0x80 : 0;
  if (config->ainp_uv_err_en)
    data[1] |= 0x40;
  if (config->ainm_ov_err_en)
    data[1] |= 0x20;
  if (config->ainm_uv_err_en)
    data[1] |= 0x10;
  if (config->ref_det_err_en)
    data[1] |= 8;
  if (config->dldo_psm_trip_test_en)
    data[1] |= 4;
  if (config->dldo_psm_err_en)
    data[1] |= 2;
  if (config->aldo_psm_trip_test_en)
    data[1] |= 1;
  data[2] = config->aldo_psm_err_en ? 0x80 : 0;
  if (config->spi_ignore_err_en)
    data[2] |= 0x40;
  if (config->spi_sclk_cnt_err_en)
    data[2] |= 0x20;
  if (config->spi_read_err_en)
    data[2] |= 0x10;
  if (config->spi_write_err_en)
    data[2] |= 8;
  if (config->spi_crc_err_en)
    data[2] |= 4;
  if (config->mm_crc_err_en)
    data[2] |= 2;
  if (config->rom_crc_err_en)
    data[2] |= 1;
  ad7124_write_register(channel, AD7124_REGISTER_ERROR_EN, data, 3);
}

int ad7124_get_offset(int channel, unsigned char channel_no)
{
  unsigned char data[3];
  ad7124_read_register(channel, AD7124_REGISTER_OFFSET0 + channel_no, data, 3);
  int offset = (data[0] << 16) | (data[1] << 8) | data[2];
  if (offset & 0x800000)
    offset |= 0xFF000000;
  return offset;
}

void ad7124_set_offset(int channel, unsigned char channel_no, int value)
{
  unsigned char data[3];
  data[0] = (unsigned char)(value >> 16);
  data[1] = (unsigned char)(value >> 8);
  data[2] = (unsigned char)value;
  ad7124_write_register(channel, AD7124_REGISTER_OFFSET0 + channel_no, data, 3);
}

int ad7124_get_gain(int channel, unsigned char channel_no)
{
  unsigned char data[3];
  ad7124_read_register(channel, AD7124_REGISTER_GAIN0 + channel_no, data, 3);
  int gain = (data[0] << 16) | (data[1] << 8) | data[2];
  if (gain & 0x800000)
    gain |= 0xFF000000;
  return gain;
}

void ad7124_set_gain(int channel, unsigned char channel_no, int value)
{
  unsigned char data[3];
  data[0] = (unsigned char)(value >> 16);
  data[1] = (unsigned char)(value >> 8);
  data[2] = (unsigned char)value;
  ad7124_write_register(channel, AD7124_REGISTER_GAIN0 + channel_no, data, 3);
}
