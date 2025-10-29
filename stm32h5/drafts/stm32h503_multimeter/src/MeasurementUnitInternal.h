#ifndef _MEASUREMENTUNITINTERNAL_H
#define _MEASUREMENTUNITINTERNAL_H

#include "multimeter.h"

class MeasurementUnitInternal: public MeasurementUint
{
public:
  MeasurementUnitInternal();
  int GetNumChannels() const override;
  int GetCurrentSourceValue(CurrentSourceLevel current_level) override;
  MultimeterChannel *GetChannel(int channel) override;
  int SetChannelCurrentSource(int channel, CurrentSourceLevel current_level) override;
  void StartMeasurement(int channel) override;
  bool IsMeasurementFinished() override;
  int GetMeasurementResult() override;
  int GetVref() override { return 0; };
  int GetMaxValue(int channel) override { return 0; };
};


#endif
