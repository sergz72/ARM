#include "MeasurementUnitADS1220.h"

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
}

int MeasurementUnitADS1220::GetNumChannels() const {return 4;}

int MeasurementUnitADS1220::GetCurrentSourceValue(CurrentSourceLevel current_level)
{
  //todo
  return 0;
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
      return NULL;
  }
}

int MeasurementUnitADS1220::SetChannelCurrentSource(int channel, CurrentSourceLevel current_level)
{
  //todo
  return 1;
}

void MeasurementUnitADS1220::StartMeasurement(int channel)
{
  //todo
}

bool MeasurementUnitADS1220::IsMeasurementFinished()
{
  //todo
  return true;
}

int MeasurementUnitADS1220::GetMeasurementResult()
{
  //todo
  return 0;
}

int MeasurementUnitADS1220::SetGain(int gain)
{
  //todo
  return 1;
}
