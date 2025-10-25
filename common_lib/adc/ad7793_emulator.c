#include <ad7793.h>
#include <ad7793_emulator.h>
#include <string.h>

ad7793_emulator_configuration ad7793_emulator_config[AD7793_MAX_CHANNELS];

static ad7793_configuration configuration[AD7793_MAX_CHANNELS];

static const unsigned int gains[8] = {1, 2, 4, 8, 16, 32, 64, 128};

void ad7793_emulator_init(void)
{
  for (int channel = 0; channel < AD7793_MAX_CHANNELS; channel++)
  {
    configuration[channel].channel = 0;
    configuration[channel].gain = 7;
    configuration[channel].boost = 0;
    configuration[channel].buffer_enable = 1;
    configuration[channel].burnout_enable = 0;
    configuration[channel].unipolar = 0;
    configuration[channel].internal_reference = 0;
    configuration[channel].vbias = 0;
  }
  memset(ad7793_emulator_config, 0, sizeof(ad7793_emulator_config));
}

static void calculate_value(int channel, unsigned char *data)
{
  int value;
  int ch = configuration[channel].channel;
  long long int uV;
  switch (ch)
  {
    case 0:
    case 1:
    case 2:
      uV = (long long int)ad7793_emulator_config[channel].ain_uv[ch] * gains[configuration[channel].gain];
      break;
    case 7:
      uV = ad7793_emulator_config[channel].vdda_mv * 167;
      break;
    default:
      uV = 0;
      break;
  }
  int vref_uv = ad7793_emulator_config[channel].vref_mv * 1000;
  if (configuration[channel].unipolar)
  {
    if (uV >= vref_uv)
      value = 0xFFFFFF;
    else
      value = (int)(uV * 0xFFFFFF / vref_uv);
  }
  else
  {
    if (uV >= vref_uv)
      value = 0xFFFFFF;
    else if (uV <= -vref_uv)
      value = 0;
    else
      value = (int)(uV * 0x7FFFFF / vref_uv) + 0x800000;
  }
  *data++ = (value >> 16) & 0xFF;
  *data++ = (value >> 8) & 0xFF;
  *data = value & 0xFF;
}

void ad7793_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength)
{
  if (!wlength)
    return;
  unsigned char register_no = (wdata[0] >> 3) & 7;
  unsigned char read = wdata[0] & 0x40;
  unsigned char write_disable = wdata[0] & 0x80;
  if (!read && write_disable)
    return;
  if (read)
  {
    if (wlength > 1 || !rlength)
      return;
    switch (register_no)
    {
      case AD7793_REGISTER_STATUS:
        if (rlength == 1)
          *rdata = 0;
        break;
      case AD7793_REGISTER_ID:
        if (rlength == 1)
          *rdata = 0x0A;
        break;
      case AD7793_REGISTER_DATA:
        if (rlength == 3)
          calculate_value(channel, rdata);
        break;
      default:
        break;
    }
  }
  else // write
  {
    switch (register_no)
    {
      case AD7793_REGISTER_CONFIGURATION:
        if (wlength == 3)
        {
          configuration[channel].channel = wdata[2] & 7;
          configuration[channel].gain = wdata[1] & 7;
          configuration[channel].boost = wdata[2] & 0x20;
          configuration[channel].buffer_enable = wdata[2] & 0x10;
          configuration[channel].burnout_enable = wdata[1] & 0x20;
          configuration[channel].unipolar = wdata[1] & 0x10;
          configuration[channel].internal_reference = wdata[2] & 0x80;
          configuration[channel].vbias = wdata[1] >> 6;
          if (configuration[channel].internal_reference)
            ad7793_emulator_config->vref_mv = 1170;
        }
        break;
      default:
        break;
    }
  }
}
