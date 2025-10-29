#ifndef _MEASUREMENTUNITAD7793_H
#define _MEASUREMENTUNITAD7793_H

#include "multimeter.h"

class MeasurementUnitAD7793: public MeasurementUint
{
  unsigned char gains[3];
public:
  explicit MeasurementUnitAD7793(void (*_setChannelCurrentSourceCallback)(int channel, CurrentSourceLevel current_level));
  int GetNumChannels() const override;
  int GetCurrentSourceValue(CurrentSourceLevel current_level) override;
  MultimeterChannel *GetChannel(int channel) override;
  int SetChannelCurrentSource(int channel, CurrentSourceLevel current_level) override;
  void StartMeasurement(int channel) override;
  bool IsMeasurementFinished() override;
  int GetMeasurementResult() override;
  int GetVref() override { return 1170; };
  int GetMaxValue(int channel) override { return channel >= 2 ? (1 << 24) - 1 : (1 << 23) - 1; };
  int SetGain(int channel, int gain) override;
  int GetGain(int channel, int gain) override;
};

extern Ohmmeter ohmmeter7793;

#endif
