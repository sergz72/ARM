#include "MeasurementUnitInternal.h"

class FrequencyMeter: public MultimeterChannel
{
public:
  MultimeterChannelType GetChannelType() override { return CHANNEL_TYPE_FREQUENCY; };
  void StartMeasurement() override { /*todo*/ }
  bool IsMeasurementFinished() override { /*todo*/ return true; }
  int GetMeasurementResult() override {/*todo*/ return 0; }
};

class CapacitanceMeter: public MultimeterChannel
{
public:
  MultimeterChannelType GetChannelType() override { return CHANNEL_TYPE_CAPACITANCE; };
  void StartMeasurement() override { /*todo*/ }
  bool IsMeasurementFinished() override { /*todo*/ return true; }
  int GetMeasurementResult() override {/*todo*/ return 0; }
};

static FrequencyMeter frequency_meter;
static CapacitanceMeter capacitance_meter;

MeasurementUnitInternal::MeasurementUnitInternal()
{
  frequency_meter.SetParameters(this, 0);
  capacitance_meter.SetParameters(this, 1);
}

int MeasurementUnitInternal::GetNumChannels() const {return 2;}

int MeasurementUnitInternal::GetCurrentSourceValue(CurrentSourceLevel current_level)
{
  return 0;
}

MultimeterChannel *MeasurementUnitInternal::GetChannel(int channel)
{
  switch (channel)
  {
    case 0:
      return &frequency_meter;
    case 1:
      return &capacitance_meter;
    default:
      return NULL;
  }
}

int MeasurementUnitInternal::SetChannelCurrentSource(int channel, CurrentSourceLevel current_level)
{
  return 1;
}

void MeasurementUnitInternal::StartMeasurement(int channel)
{
}

bool MeasurementUnitInternal::IsMeasurementFinished()
{
  return false;
}

int MeasurementUnitInternal::GetMeasurementResult()
{
  return 0;
}
