#ifndef _MEASUREMENTUNITAD7793_H
#define _MEASUREMENTUNITAD7793_H

#include "multimeter.h"

class MeasurementUnitAD7793: public MeasurementUint
{
public:
  MeasurementUnitAD7793();
  int GetNumChannels() const override;
  long long int GetCurrentSourceValue(CurrentSourceLevel current_level) override;
  MultimeterChannel *GetChannel(int channel) override;
  int SetChannelCurrentSource(int channel, CurrentSourceLevel current_level) override;
  void StartMeasurement(int channel, unsigned int parameter) override;
  bool IsMeasurementFinished() override;
  long long int GetMeasurementResult() override;
};


#endif
