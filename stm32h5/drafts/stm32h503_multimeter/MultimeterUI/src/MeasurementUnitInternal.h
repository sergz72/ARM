#ifndef _MEASUREMENTUNITINTERNAL_H
#define _MEASUREMENTUNITINTERNAL_H

#include "multimeter.h"

class MeasurementUnitInternal: public MeasurementUint
{
public:
  MeasurementUnitInternal();
  int GetNumChannels() const override;
  long long int GetCurrentSourceValue(CurrentSourceLevel current_level) override;
  MultimeterChannel *GetChannel(int channel) override;
  int SetChannelCurrentSource(int channel, CurrentSourceLevel current_level) override;
  void StartMeasurement(int channel, unsigned int parameter) override;
  bool IsMeasurementFinished() override;
  long long int GetMeasurementResult() override;
};


#endif
