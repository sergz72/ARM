#include "MeasurementUnitADS1220.h"
#include <ads1220.h>

class Thermometer: public MultimeterChannel
{
public:
  MultimeterChannelType GetChannelType() override { return CHANNEL_TYPE_TEMPERATURE; };
  int GetMeasurementResult() override { return measurement_unit->GetMeasurementResult() * 10 / 32; };
};

static Ampermeter ampermeter(1000);
static Voltmeter voltmeter(10);
static Ohmmeter ohmmeter;
static Thermometer thermometer;

MeasurementUnitADS1220::MeasurementUnitADS1220(void (*_setChannelCurrentSourceCallback)(int channel, CurrentSourceLevel current_level))
  : MeasurementUint(_setChannelCurrentSourceCallback)
{
  ampermeter.SetParameters(this, ADS1220_MUX_AIN2_AIN3);
  voltmeter.SetParameters(this, ADS1220_MUX_AIN1_AIN2);
  ohmmeter.SetParameters(this, ADS1220_MUX_AIN0_AVSS);
  thermometer.SetParameters(this, ADS1220_MUX_SHORT);
  gains[0] = gains[1] = gains[2] = 1;
  temperature_sensor_mode = false;
}

int MeasurementUnitADS1220::GetNumChannels() const {return 4;}

int MeasurementUnitADS1220::GetCurrentSourceValue(CurrentSourceLevel current_level)
{
  switch (current_level)
  {
    case CURRENT_LEVEL_LO: return 10;
    case CURRENT_LEVEL_MID: return 100;
    default: return 1000;
  }
}

MultimeterChannel *MeasurementUnitADS1220::GetChannel(int channel)
{
  switch (channel)
  {
    case 0:
      return &ampermeter;
    case 1:
      return &voltmeter;
    case 2:
      return &ohmmeter;
    case 3:
      return &thermometer;
    default:
      return nullptr;
  }
}

int MeasurementUnitADS1220::SetChannelCurrentSource(int channel, CurrentSourceLevel current_level)
{
  if (setChannelCurrentSourceCallback)
    setChannelCurrentSourceCallback(channel, current_level);

  if (channel != ADS1220_MUX_AIN0_AVSS)
    return 1;
  unsigned char current;
  switch (current_level)
  {
    case CURRENT_LEVEL_LO: current = ADS1220_IDAC_10;
    case CURRENT_LEVEL_MID: current = ADS1220_IDAC_100;
    default: current = ADS1220_IDAC_1000;
  }
  ads1220_set_idac(0, current, ADS1220_IDAC_MUX_REFP0, ADS1220_IDAC_MUX_OFF);
  return 0;
}

void MeasurementUnitADS1220::StartMeasurement(int channel)
{
  if (channel == ADS1220_MUX_SHORT)
  {
    ads1220_enter_temperature_sensor_mode(0);
    temperature_sensor_mode = true;
  }
  ads1220_read_start(0, channel, gains[channel], gains[channel] <= 4);
}

bool MeasurementUnitADS1220::IsMeasurementFinished()
{
  return ADS1220_DRDY_GET(channel) == 0;
}

int MeasurementUnitADS1220::GetMeasurementResult()
{
  int value = ads1220_read_finish();
  if (temperature_sensor_mode)
  {
    ads1220_leave_temperature_sensor_mode(0);
    temperature_sensor_mode = false;
  }
  return value;
}

int MeasurementUnitADS1220::GetGain(int channel, int gain)
{
  if (gain < 2)
    return 1;
  if (gain < 4)
    return 2;

  if (channel == ADS1220_MUX_AIN0_AVSS) // ohmmeter
    return 4;

  if (gain < 8)
    return 4;
  if (gain < 16)
    return 8;
  if (gain < 32)
    return 16;
  if (gain < 64)
    return 32;
  if (gain < 128)
    return 64;
  return 128;
}

int MeasurementUnitADS1220::SetGain(int channel, int gain)
{
  if (gain < 2)
    gain = 0;
  else if (gain < 4)
    gain = 1;
  else if (gain < 8)
    gain = 2;
  else if (gain < 16)
    gain = 3;
  else if (gain < 32)
    gain = 4;
  else if (gain < 64)
    gain = 5;
  else if (gain < 128)
    gain = 6;
  else
    gain = 7;

  if (channel == ADS1220_MUX_AIN0_AVSS && gain > 2) // ohmmeter
    gain = 2;

  gains[channel] = gain;
  return 1 << gain;
}
