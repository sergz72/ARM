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
static Ohmmeter ohmmeter;

MeasurementUnitAD7793::MeasurementUnitAD7793()
{
  ampermeter.SetParameters(this, AD7793_CHANNEL_AIN1P_AIN1N);
  voltmeter.SetParameters(this, AD7793_CHANNEL_AIN2P_AIN2N);
  ohmmeter.SetParameters(this, AD7793_CHANNEL_AIN3P_AIN3N);
  voltmeter_vdda.SetParameters(this, AD7793_CHANNEL_AVDD6);
}

int MeasurementUnitAD7793::GetNumChannels() const {return 4;}

int MeasurementUnitAD7793::GetCurrentSourceValue(CurrentSourceLevel current_level)
{
  //todo
  return 0;
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
      return &ohmmeter;
    case 3:
      return &voltmeter_vdda;
    default:
      return NULL;
  }
}

int MeasurementUnitAD7793::SetChannelCurrentSource(int channel, CurrentSourceLevel current_level)
{
  //todo
  return 1;
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

  gains[channel] = gain;
  return 1 << gain;
}
