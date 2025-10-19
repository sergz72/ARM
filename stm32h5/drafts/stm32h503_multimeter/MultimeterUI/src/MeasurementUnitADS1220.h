#ifndef _MEASUREMENTUNITADS1220_H
#define _MEASUREMENTUNITADS1220_H

#include "multimeter.h"

class MeasurementUnitADS1220: public MeasurementUint
{
public:
  explicit MeasurementUnitADS1220();
  int GetNumChannels() const override;
  long long int GetCurrentSourceValue(CurrentSourceLevel current_level) override;
  MultimeterChannel *GetChannel(int channel) override;
  int SetChannelCurrentSource(int channel, CurrentSourceLevel current_level) override;
  void StartMeasurement(int channel, unsigned int parameter) override;
  bool IsMeasurementFinished() override;
  long long int GetMeasurementResult() override;
};


#endif
