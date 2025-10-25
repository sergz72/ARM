#include <ad7793.h>
#include "MeasurementUnitAD7793.h"

static Ampermeter ampermeter(1000);
static Voltmeter voltmeter(10);
static Voltmeter voltmeter_vdda(CHANNEL_TYPE_VDDA, 4);
static Ohmmeter ohmmeter;

MeasurementUnitAD7793::MeasurementUnitAD7793()
{
  ampermeter.SetParameters(this, 0);
  voltmeter.SetParameters(this, 1);
  ohmmeter.SetParameters(this, 3);
  voltmeter_vdda.SetParameters(this, 7);
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
  ad7793_read_start(0, channel, gains[channel], 0);
}

bool MeasurementUnitAD7793::IsMeasurementFinished()
{
  return true;
}

int MeasurementUnitAD7793::GetMeasurementResult()
{
  return ad7793_read_finish(0) - 0x800000;
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
