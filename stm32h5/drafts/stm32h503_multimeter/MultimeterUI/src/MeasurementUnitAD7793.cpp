#include <ad7793.h>
#include "MeasurementUnitAD7793.h"

class VDDAMeter: public MultimeterChannel
{
public:
  MultimeterChannelType GetChannelType() override { return CHANNEL_TYPE_VDDA; };
  int GetMeasurementResult() override
  {
    return static_cast<int>(static_cast<long long int>(measurement_unit->GetMeasurementResult()) * measurement_unit->GetVref() * 6000 /
                            measurement_unit->GetMaxValue(channel_no));
  };
};

static Ampermeter ampermeter(1000);
static Voltmeter voltmeter(10);
static VDDAMeter voltmeter_vdda;

Ohmmeter ohmmeter7793;

MeasurementUnitAD7793::MeasurementUnitAD7793(void (*_setChannelCurrentSourceCallback)(int channel, CurrentSourceLevel current_level))
  : MeasurementUint(_setChannelCurrentSourceCallback)
{
  ampermeter.SetParameters(this, AD7793_CHANNEL_AIN1P_AIN1N);
  voltmeter.SetParameters(this, AD7793_CHANNEL_AIN2P_AIN2N);
  ohmmeter7793.SetParameters(this, AD7793_CHANNEL_AIN3P_AIN3N);
  voltmeter_vdda.SetParameters(this, AD7793_CHANNEL_AVDD6);
}

int MeasurementUnitAD7793::GetNumChannels() const {return 4;}

int MeasurementUnitAD7793::GetCurrentSourceValue(CurrentSourceLevel current_level)
{
  switch (current_level)
  {
    case CURRENT_LEVEL_LO: return 10;
    case CURRENT_LEVEL_MID: return 210;
    default: return 1000;
  }
}

MultimeterChannel *MeasurementUnitAD7793::GetChannel(int channel)
{
  switch (channel)
  {
    case 0:
      return &ampermeter;
    case 1:
      return &voltmeter;
    case 2:
      return &ohmmeter7793;
    case 3:
      return &voltmeter_vdda;
    default:
      return NULL;
  }
}

int MeasurementUnitAD7793::SetChannelCurrentSource(int channel, CurrentSourceLevel current_level)
{
  if (setChannelCurrentSourceCallback)
    setChannelCurrentSourceCallback(channel, current_level);
  if (channel != AD7793_CHANNEL_AIN3P_AIN3N)
    return 1;
  unsigned char current;
  switch (current_level)
  {
    case CURRENT_LEVEL_LO: current = AD7793_IEXCEN_10;
    case CURRENT_LEVEL_MID: current = AD7793_IEXCEN_210;
    default: current = AD7793_IEXCEN_1000;
  }
  ad7793_set_io(0, AD7793_IEXCDIR_IEXC1_IOUT1_IEXC2_IOUT2, current);
  return 0;
}

void MeasurementUnitAD7793::StartMeasurement(int channel)
{
  ad7793_read_start(0, channel, gains[channel], channel >= 2);
}

bool MeasurementUnitAD7793::IsMeasurementFinished()
{
  return AD7793_RDY_GET(0) == 0;
}

int MeasurementUnitAD7793::GetMeasurementResult()
{
  return ad7793_read_finish(0);
}

int MeasurementUnitAD7793::GetGain(int channel, int gain)
{
  if (gain < 2)
    return 1;
  if (gain < 4)
    return 2;

  if (channel == AD7793_CHANNEL_AIN3P_AIN3N) // ohmmeter
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

int MeasurementUnitAD7793::SetGain(int channel, int gain)
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

  if (channel == AD7793_CHANNEL_AIN3P_AIN3N && gain > 2) // ohmmeter
    gain = 2;

  gains[channel] = gain;
  return 1 << gain;
}
