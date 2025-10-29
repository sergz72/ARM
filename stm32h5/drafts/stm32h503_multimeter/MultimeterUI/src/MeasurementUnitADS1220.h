#ifndef _MEASUREMENTUNITADS1220_H
#define _MEASUREMENTUNITADS1220_H

#include "multimeter.h"

class MeasurementUnitADS1220: public MeasurementUint
{
  unsigned char gains[15];
  bool temperature_sensor_mode;
public:
  explicit MeasurementUnitADS1220(void (*_setChannelCurrentSourceCallback)(int channel, CurrentSourceLevel current_level));
  int GetNumChannels() const override;
  int GetCurrentSourceValue(CurrentSourceLevel current_level) override;
  MultimeterChannel *GetChannel(int channel) override;
  int SetChannelCurrentSource(int channel, CurrentSourceLevel current_level) override;
  void StartMeasurement(int channel) override;
  bool IsMeasurementFinished() override;
  int GetMeasurementResult() override;
  int GetVref() override { return 2500; };
  int GetMaxValue(int channel) override { return (1 << 23) - 1; };
  int SetGain(int channel, int gain) override;
  int GetGain(int channel, int gain) override;
};

extern Ohmmeter ohmmeter1220;

#endif
