#ifndef _MEASUREMENTUNITAD7793_H
#define _MEASUREMENTUNITAD7793_H

#include "multimeter.h"

class MeasurementUnitAD7793: public MeasurementUint
{
public:
  MeasurementUnitAD7793();
  int GetNumChannels() const override;
  int GetCurrentSourceValue(CurrentSourceLevel current_level) override;
  MultimeterChannel *GetChannel(int channel) override;
  int SetChannelCurrentSource(int channel, CurrentSourceLevel current_level) override;
  void StartMeasurement(int channel) override;
  bool IsMeasurementFinished() override;
  int GetMeasurementResult() override;
  int GetVref() override { return 1170; };
  int GetMaxValue(int channel) override { return (1 << 24) - 1; };
  int SetGain(int gain) override;
};


#endif
