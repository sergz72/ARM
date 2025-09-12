#include "board.h"
#include <ads1259.h>
#include <string.h>

#define ADS1259_OFFSET_CALIBRATION_COMMAND 0x18
#define ADS1259_GAIN_CALIBRATION_COMMAND   0x19
#define ADS1259_WRITE_REGISTER_COMMAND(regiser) (0x40 | regiser)
#define ADS1259_READ_REGISTER_COMMAND(regiser) (0x20 | regiser)

#define ADS1259_REGISTER_CONFIG0 0
#define ADS1259_REGISTER_CONFIG1 1
#define ADS1259_REGISTER_CONFIG2 2
#define ADS1259_REGISTER_OFC0    3
#define ADS1259_REGISTER_OFC1    4
#define ADS1259_REGISTER_OFC2    5
#define ADS1259_REGISTER_FSC0    6
#define ADS1259_REGISTER_FSC1    7
#define ADS1259_REGISTER_FSC2    8

void ads1259_reset(void)
{
  delayms(100);
  for (int channel = 0; channel < ADS1259_MAX_CHANNELS; channel++)
    ADS1259_RESET_SET(channel);
  delayms(1);
}

static void ads1259_command(int channel, unsigned char command)
{
  ads1259_spi_transfer(channel, &command, 1, NULL, 0);
}

static void ads1259_read_registers(int channel, unsigned short reg_no, unsigned short length, unsigned char *data)
{
  unsigned short command = ADS1259_READ_REGISTER_COMMAND(reg_no) | (length << 8);
  ads1259_spi_transfer(channel, (unsigned char*)&command, 2, data, length);
}

static void ads1259_write_registers(int channel, unsigned short reg_no, unsigned short length, const unsigned char *data)
{
  unsigned char wdata[2+length];
  wdata[0] = ADS1259_WRITE_REGISTER_COMMAND(reg_no);
  wdata[1] = length;
  memcpy(wdata+2, data, length);
  ads1259_spi_transfer(channel, wdata, length+2, NULL, 0);
}

static int ads1259_wait(int channel, int timeout)
{
  while (timeout)
  {
    delayms(1);
    if (!ADS1259_DRDY_GET(channel))
      break;
    timeout--;
  }
  return timeout == 0;
}

void ads1259_init(int channel, const ads1259_configuration *config)
{
  unsigned char values[3];
  values[0] = config->internal_reference_bias_enable ? 4 : 0;
  if (config->spi_timeout_enable)
    values[0] |= 1;

  values[1] = config->conversion_delay & 7;
  if (config->out_of_range_flag_enable)
    values[1] |= 0x80;
  if (config->checksum_enable)
    values[1] |= 0x40;
  if (config->digital_filter_sinc2)
    values[1] |= 0x10;
  if (config->external_reference_enable)
    values[1] |= 8;

  values[2] = config->data_rate & 7;
  if (config->syncout_enable)
    values[2] |= 0x20;
  if (config->pulse_mode)
    values[2] |= 0x10;

  ads1259_write_registers(channel, ADS1259_REGISTER_CONFIG0, 3, values);
}

static unsigned char ads1259_clear_pulse(int channel)
{
  unsigned char config2;
  ads1259_read_registers(channel, ADS1259_REGISTER_CONFIG2, 1, &config2);
  if (config2 & 0x10)
  {
    unsigned char newconfig2 = config2 & 0xEF; // clear pulse
    ads1259_write_registers(channel, ADS1259_REGISTER_CONFIG2, 1, &newconfig2);
  }
  return config2;
}

static void ads1259_restore_pulse(int channel, unsigned char config2)
{
  if (config2 & 0x10)
    ads1259_write_registers(channel, ADS1259_REGISTER_CONFIG2, 1, &config2);
}

int ads1259_calibrate_offset2(int channel, int timeout)
{
  unsigned char config2 = ads1259_clear_pulse(channel);
  ADS1259_START_SET(channel);
  ads1259_command(channel, ADS1259_OFFSET_CALIBRATION_COMMAND);
  int rc = ads1259_wait(channel, timeout);
  ADS1259_START_CLR(channel);
  ads1259_restore_pulse(channel, config2);
  return rc;
}

int ads1259_read(int channel, int *pvalue, int timeout)
{
  ADS1259_START_SET(channel);
  int rc = ads1259_wait(channel, timeout);
  if (rc)
    return rc;
  ADS1259_START_CLR(channel);
  unsigned char cvalue[3];
  ads1259_spi_transfer(channel, NULL, 0, cvalue, 3);
  int value = (cvalue[0] << 16) | (cvalue[1] << 8) | cvalue[2];
  if (value & 0x800000)
    value |= 0xFF000000;
  *pvalue = value;
  return 0;
}

int ads1259_read_voltage(int channel, double input_divider_coefficient, double vref, double *result, int timeout)
{
  int value;
  int rc = ads1259_read(channel, &value, timeout);
  if (rc)
    return rc;
  *result = (double)value * vref / (double)0x7FFFFF / input_divider_coefficient;
  return 0;
}

int ads1259_calibrate_offset(int channel, int num_loops, int timeout)
{
  int ofc = 0;
  ads1259_read_registers(channel, ADS1259_REGISTER_OFC0, 3, (unsigned char*)&ofc);
  if (ofc & 0x800000)
    ofc |= 0xFF000000;
  for (int i = 0; i < num_loops; i++)
  {
    int value;
    int rc = ads1259_read(channel, &value, timeout);
    if (rc)
      return rc;
    if (value == 0)
      break;
    ofc += value;
    ads1259_write_registers(channel, ADS1259_REGISTER_OFC0, 3, (unsigned char*)&ofc);
  }
  return 0;
}

void ads1259_set_fsc(int channel, int fsc)
{
  ads1259_write_registers(channel, ADS1259_REGISTER_FSC0, 3, (unsigned char*)&fsc);
}

static int calculate_expected_value(double expected_voltage, double input_divider_coefficient, double vref)
{
  return (int)(expected_voltage * input_divider_coefficient * (double)0x7FFFFF / vref);
}

int ads1259_calibrate_gain(ads1259_gain_calibration *calibration, int timeout)
{
  if (calibration->expected_voltage == 0)
    return 1;
  calibration->expected_value = calculate_expected_value(calibration->expected_voltage, calibration->input_divider_coefficient, calibration->vref);
  if (calibration->expected_value == 0)
    return 3;
  calibration->fsc = 0x400000;
  ads1259_read_registers(calibration->channel, ADS1259_REGISTER_FSC0, 3, (unsigned char*)&calibration->fsc);
  for (int i = 0; i < calibration->num_loops; i++)
  {
    int rc = ads1259_read(calibration->channel, &calibration->result, timeout);
    if (rc)
      return rc;
    if (calibration->result == 0)
      return 2;
    int difference = calibration->result - calibration->expected_value;
    if (difference == 0)
      break;
    calibration->fsc = (int)((long long int)calibration->fsc * (long long int)calibration->expected_value / calibration->result);
    ads1259_write_registers(calibration->channel, ADS1259_REGISTER_FSC0, 3, (unsigned char*)&calibration->fsc);
  }
  return 0;
}