#include "MeasurementUnitADS1220.h"
#include <ads1220.h>

class Thermometer: public MultimeterChannel
{
public:
  MultimeterChannelType GetChannelType() override { return CHANNEL_TYPE_TEMPERATURE; };
  int GetMeasurementResult() override { /*todo*/ return 0; };
};

static Ampermeter ampermeter(1000);
static Voltmeter voltmeter(10);
static Ohmmeter ohmmeter;
static Thermometer thermometer;

MeasurementUnitADS1220::MeasurementUnitADS1220()
{
  ampermeter.SetParameters(this, 0);
  voltmeter.SetParameters(this, 1);
  ohmmeter.SetParameters(this, 2);
  thermometer.SetParameters(this, 3);
  gains[0] = gains[1] = gains[2] = 1;
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
  if (channel != 2)
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
  ads1220_read_start(0, channel, gains[channel], gains[channel] <= 4);
}

bool MeasurementUnitADS1220::IsMeasurementFinished()
{
  return true;
}

int MeasurementUnitADS1220::GetMeasurementResult()
{
  return ads1220_read_finish();
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

  gains[channel] = gain;
  return 1 << gain;
}
