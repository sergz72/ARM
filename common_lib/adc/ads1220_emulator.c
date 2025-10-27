#include <ads1220_emulator.h>
#include <ads1220.h>
#include <string.h>

#define AVSS 4
#define AVDD4 5
#define VREFP4 6

ads1220_emulator_configuration ads1220_emulator_config[ADS1220_MAX_CHANNELS];

static ads1220_configuration configuration[ADS1220_MAX_CHANNELS];

static const unsigned int gains[8] = {1, 2, 4, 8, 16, 32, 64, 128};

static const unsigned int mux_to_ainp[ADS1220_MAX_MUX+1] = {0, 0, 0, 1, 1, 2, 1, 3, 0, 1, 2, 3, VREFP4, AVDD4, AVSS};
static const unsigned int mux_to_ainm[ADS1220_MAX_MUX+1] = {1, 2, 3, 2, 3, 3, 0, 2, AVSS, AVSS, AVSS, AVSS, AVSS, AVSS, AVSS};

void ads1220_emulator_init(void)
{
  memset(configuration, 0, sizeof(configuration));
  memset(ads1220_emulator_config, 0, sizeof(ads1220_emulator_config));
}

static void calculate_value(int channel, unsigned char *data)
{
  int value;
  unsigned char mux = configuration[channel].mux;
  unsigned int ainp = mux_to_ainp[mux];
  unsigned int ainm = mux_to_ainm[mux];
  int vainp;
  switch (ainp)
  {
  case AVDD4:
    vainp = ads1220_emulator_config->vdda_mv * 250;
    break;
  case VREFP4:
    vainp = ads1220_emulator_config->vref_mv * 250;
    break;
  case AVSS:
    vainp = 0;
  default:
    vainp = ads1220_emulator_config[channel].ain_uv[ainp];
    break;
  }
  int vainm = ainm == AVSS ? 0 : ads1220_emulator_config[channel].ain_uv[ainm];

  long long int uV = (long long int)(vainp-vainm) * gains[configuration[channel].gain];
  int vref_uv = ads1220_emulator_config[channel].vref_mv * 1000;
  if (uV >= vref_uv)
    value = 0x7FFFFF;
  else if (uV <= -vref_uv)
    value = 0x800000;
  else
    value = (int)(uV * 0x7FFFFF / vref_uv);
  *data++ = (value >> 16) & 0xFF;
  *data++ = (value >> 8) & 0xFF;
  *data = value & 0xFF;
}

static void calculate_temperature(int channel, unsigned char *data)
{
  int value = ads1220_emulator_config[channel].temperature * 32 / 10;
  *data++ = (value >> 16) & 0xFF;
  *data++ = (value >> 8) & 0xFF;
  *data = value & 0xFF;
}

void write_register(int channel, unsigned char register_no, unsigned char data)
{
  switch (register_no)
  {
    case ADS1220_REGISTER_CONFIG0:
      configuration[channel].mux = data >> 4;
      configuration[channel].gain = (data >> 1) & 7;
      configuration[channel].pga_bypass = data & 1;
      break;
    case ADS1220_REGISTER_CONFIG1:
      configuration[channel].data_rate = data >> 5;
      configuration[channel].mode = (data >> 3) & 3;
      configuration[channel].continuous_conversion_mode = data & 4;
      configuration[channel].temperature_sensor_mode = data & 2;
      configuration[channel].burnout_sources_enable = data & 1;
      break;
    case ADS1220_REGISTER_CONFIG2:
      configuration[channel].vref = data >> 6;
      configuration[channel].filter_50_60 = (data >> 4) & 3;
      configuration[channel].idac_current = data & 7;
      configuration[channel].auto_close_low_side_switch = data & 8;
      if (configuration[channel].vref == 0) // internal reference
        ads1220_emulator_config[channel].vref_mv = 2500;
      break;
    case ADS1220_REGISTER_CONFIG3:
      configuration[channel].idac1_mux = data >> 5;
      configuration[channel].idac2_mux = (data >> 2) & 7;
      configuration[channel].data_ready_on_dout = data & 2;
      break;
    default:
      break;
  }
}

void ads1220_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength)
{
  if (!wlength)
  {
    if (rlength != 3)
      return;
    if (configuration->temperature_sensor_mode)
      calculate_temperature(channel, rdata);
    else
      calculate_value(channel, rdata);
    return;
  }
  if ((wdata[0] & 0xF0) != ADS1220_COMMAND_WREG)
    return;
  unsigned char register_no = (wdata[0] >> 2) & 3;
  unsigned char nn = (wdata[0] & 3) + 1;
  int i = 1;
  wlength--;
  while (nn-- && wlength--)
    write_register(channel, register_no++, wdata[i++]);
}
