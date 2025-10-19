#include "MeasurementUnitAD7793.h"

static Ampermeter ampermeter(1000);
static Voltmeter voltmeter(10);
static Voltmeter voltmeter_vdda(4);
static Ohmmeter ohmmeter;

MeasurementUnitAD7793::MeasurementUnitAD7793()
{
  ampermeter.SetParameters(this, 0);
  voltmeter.SetParameters(this, 1);
  ohmmeter.SetParameters(this, 3);
  voltmeter_vdda.SetParameters(this, 4);
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
  //todo
}

bool MeasurementUnitAD7793::IsMeasurementFinished()
{
  //todo
  return true;
}

int MeasurementUnitAD7793::GetMeasurementResult()
{
  //todo
  return 0;
}

int MeasurementUnitAD7793::SetGain(int gain)
{
  //todo
  return 1;
}
