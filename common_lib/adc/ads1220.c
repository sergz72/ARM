#include "board.h"
#include <ads1220.h>
#include <string.h>

static unsigned char configurations[AD7793_MAX_CHANNELS][4];
static int current_channel;
static unsigned char current_channel_no, current_gain;

int ads1220_offsets[AD7793_MAX_CHANNELS][ADS1220_MAX_MUX+1][ADS1220_MAX_GAIN+1];

void ads1220_init(void)
{
  memset(configurations, 0, sizeof(configurations));
  memset(ads1220_offsets, 0, sizeof(ads1220_offsets));
}

static void ads1220_send_command(int channel, unsigned char command)
{
  ads1220_spi_transfer(channel, &command, 1, NULL, 0);
}

void ads1220_read_registers(int channel, unsigned char reg_no, unsigned char *data, unsigned char length)
{
  if (!length)
    return;
  if (length > 3)
    length = 3;
  reg_no = ((reg_no & 3) << 2) | (length-1) | ADS1220_COMMAND_RREG;
  ads1220_spi_transfer(channel, &reg_no, 1, data, length);
}

void ads1220_write_registers(int channel, unsigned char reg_no, const unsigned char *data, unsigned char length)
{
  if (!length)
    return;
  if (length > 3)
    length = 3;
  unsigned char l = length + 1;
  unsigned char wdata[l];
  wdata[0] = ((reg_no & 3) << 2) | (length-1) | ADS1220_COMMAND_WREG;
  memcpy(wdata+1, data, length);
  ads1220_spi_transfer(channel, wdata, l, NULL, 0);
}

void ads1220_reset(int channel)
{
  ads1220_send_command(channel, ADS1220_COMMAND_RESET);
  delay(100);
}

static void ads1220_set_channel_and_gain(int channel, unsigned char channel_no, unsigned char gain, int pga_bypass)
{
  unsigned char data;
  data = (channel_no << 4) | (gain << 1) | (pga_bypass ? 1 : 0);
  ads1220_write_registers(channel, ADS1220_REGISTER_CONFIG0, &data, 1);
  configurations[channel][0] = data;
}

void ads1220_set_configuration(int channel, const ads1220_configuration *config)
{
  unsigned char data[4];
  data[0] = (config->mux << 4) | (config->gain << 1) | (config->pga_bypass ? 1 : 0);
  data[1] = (config->data_rate << 5) | (config->mode << 3);
  if (config->continuous_conversion_mode)
    data[1] |= 4;
  if (config->temperature_sensor_mode)
    data[1] |= 2;
  if (config->burnout_sources_enable)
    data[1] |= 1;
  data[2] = (config->vref << 6) | (config->filter_50_60 << 4) | config->idac_current | (config->auto_close_low_side_switch ? 8 : 0);
  data[3] = (config->idac1_mux << 5) | (config->idac2_mux << 2) | (config->data_ready_on_dout ? 2 : 0);
  ads1220_write_registers(channel, ADS1220_REGISTER_CONFIG0, data, 4);
  configurations[channel][0] = data[0];
  configurations[channel][1] = data[1];
  configurations[channel][2] = data[2];
  configurations[channel][3] = data[3];
}

static int ads1220_wait(int channel, int timeout)
{
  while (timeout)
  {
    delayms(1);
    if (!ADS1220_DRDY_GET(channel))
      break;
    timeout--;
  }
  return timeout == 0;
}

void ads1220_read_start(int channel, unsigned char channel_no, unsigned char gain, int pga_bypass)
{
  unsigned char data[3];
  if (!ADS1220_DRDY_GET(channel))
    ads1220_spi_transfer(channel, NULL, 0, data, 3);
  ads1220_set_channel_and_gain(channel, channel_no, gain, pga_bypass);
  ads1220_send_command(channel, ADS1220_COMMAND_START);
  current_channel = channel;
  current_channel_no = channel_no;
  current_gain = gain;
}

int ads1220_read_finish(void)
{
  unsigned char data[3];
  ads1220_spi_transfer(current_channel, NULL, 0, data, 3);
  int value = (data[0] << 16) | (data[1] << 8) | data[2];
  if (value & 0x800000)
    value |= 0xFF000000;
  return value - ads1220_offsets[current_channel][current_channel_no][current_gain];
}

int ads1220_read(int channel, unsigned char channel_no, unsigned char gain, int pga_bypass, int *result, int timeout)
{
  ads1220_read_start(channel, channel_no, gain, pga_bypass);
  int rc = ads1220_wait(channel, timeout);
  if (rc)
    return rc;
  *result = ads1220_read_finish();
  return 0;
}

void ads1220_read_voltage_finish(const ads1220_read_voltage_configuration * configuration,
                                  ads1220_data *result)
{
  result->value = ads1220_read_finish();
  result->voltage = (double)result->value * configuration->vref / (double)0x7FFFFF /
                    configuration->input_divider_coefficient / (1<<configuration->gain);
}

int ads1220_read_voltage(const ads1220_read_voltage_configuration * configuration,
                        ads1220_data *result, int timeout)
{
  ads1220_read_start(configuration->channel, configuration->channel_no, configuration->gain,
                        configuration->pga_bypass);
  int rc = ads1220_wait(configuration->channel, timeout);
  if (rc)
    return rc;
  ads1220_read_voltage_finish(configuration, result);
  return 0;
}

void ads1220_set_idac(int channel, unsigned char idac_current, unsigned char idac1_mux, unsigned char idac2_mux)
{
  unsigned char data[2];
  data[0] = (configurations[channel][2] & 0xF8) | idac_current;
  data[1] = (idac1_mux << 5) | (idac2_mux << 2) | (configurations[channel][3] & 2);
  ads1220_write_registers(channel, ADS1220_REGISTER_CONFIG2, data, 2);
  configurations[channel][2] = data[0];
  configurations[channel][3] = data[1];
}

static void ads1220_enter_temperature_sensor_mode(int channel)
{
  unsigned char data;
  data = configurations[channel][1] | 2;
  ads1220_write_registers(channel, ADS1220_REGISTER_CONFIG1, &data, 1);
  configurations[channel][1] = data;
}

static void ads1220_leave_temperature_sensor_mode(int channel)
{
  unsigned char data;
  data = configurations[channel][1] & 0xFD;
  ads1220_write_registers(channel, ADS1220_REGISTER_CONFIG1, &data, 1);
  configurations[channel][1] = data;
}

int ads1220_read_temperature(int channel, ads1220_data *result, int timeout)
{
  ads1220_enter_temperature_sensor_mode(channel);
  int rc = ads1220_read(channel, ADS1220_MUX_AIN0_AIN1, ADS1220_PGA_GAIN_1, 1, &result->value, timeout);
  ads1220_leave_temperature_sensor_mode(channel);
  if (!rc)
    result->voltage = (double)(result->value >> 10) * 0.03125;
  return rc;
}

int ads1220_calibrate_offset(int channel, unsigned char channel_no, int steps)
{
  for (unsigned char gain = 0; gain <= ADS1220_MAX_GAIN; gain++)
  {
    int sum = 0;
    for (int step = 0; step < steps; step++)
    {
      int value;
      int rc = ads1220_read(channel, channel_no, gain, gain < 3, &value, 1000);
      if (rc)
        return rc;
      sum += value;
    }
    ads1220_offsets[channel][channel_no][gain] = sum / steps;
  }
  return 0;
}
