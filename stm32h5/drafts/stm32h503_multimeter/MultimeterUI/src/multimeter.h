#ifndef MULTIMETER_H
#define MULTIMETER_H

#include "board.h"

#ifndef MULTIMETER_MAX_CHANNELS_PER_CHANNEL_TYPE
#define MULTIMETER_MAX_CHANNELS_PER_CHANNEL_TYPE 2
#endif

#ifndef MULTIMETER_MAX_CHANNELS_PER_UNIT
#define MULTIMETER_MAX_CHANNELS_PER_UNIT 4
#endif

typedef enum
{
  CHANNEL_TYPE_NONE,
  CHANNEL_TYPE_VOLTAGE,
  CHANNEL_TYPE_CURRENT,
  CHANNEL_TYPE_RESISTANCE,
  CHANNEL_TYPE_TEMPERATURE,
  CHANNEL_TYPE_VDDA,
  CHANNEL_TYPE_CAPACITANCE,
  CHANNEL_TYPE_FREQUENCY,
  CHANNEL_TYPE_MAX = CHANNEL_TYPE_FREQUENCY
} MultimeterChannelType;

typedef enum
{
  CURRENT_LEVEL_LO,
  CURRENT_LEVEL_MID,
  CURRENT_LEVEL_HI,
  CURRENT_LEVEL_MAX = CURRENT_LEVEL_HI
} CurrentSourceLevel;

class MeasurementUint;

class MultimeterChannel
{
protected:
  MeasurementUint *measurement_unit;
  int channel_no;
public:
  void SetParameters(MeasurementUint *_measurement_unit, int _channel_no);
  virtual ~MultimeterChannel() = default;
  virtual MultimeterChannelType GetChannelType() = 0;
  virtual void StartMeasurement();
  virtual bool IsMeasurementFinished();
  virtual long long int GetMeasurementResult() = 0;
  virtual bool IsReadyForNewMeasurement();
};

class Ampermeter: public MultimeterChannel
{
  unsigned int R;
public:
  Ampermeter(long long int _R);
  MultimeterChannelType GetChannelType() override;
  long long int GetMeasurementResult() override;
};

class Voltmeter: public MultimeterChannel
{
  long long int coef;
public:
  Voltmeter(long long int _coef);
  MultimeterChannelType GetChannelType() override;
  long long int GetMeasurementResult() override;
};

class Ohmmeter: public MultimeterChannel
{
  CurrentSourceLevel current_level;
public:
  Ohmmeter();
  MultimeterChannelType GetChannelType() override;
  bool IsMeasurementFinished() override;
  long long int GetMeasurementResult() override;
};

class Multimeter;

class MeasurementUint
{
  Multimeter *multimeter;
public:
  void SetParameters(Multimeter *_multimeter) { multimeter = _multimeter; }
  virtual ~MeasurementUint() = default;
  virtual int GetNumChannels() const = 0;
  virtual long long int GetCurrentSourceValue(CurrentSourceLevel current_level) = 0;
  virtual MultimeterChannel *GetChannel(int channel) = 0;
  virtual int SetChannelCurrentSource(int channel, CurrentSourceLevel current_level) = 0;
  virtual void StartMeasurement(int channel, unsigned int parameter) = 0;
  virtual bool IsMeasurementFinished() = 0;
  virtual long long int GetMeasurementResult() = 0;
  unsigned int GetTicks(unsigned int ms) const;
};

class Multimeter
{
  MeasurementUint **measurement_units;
  int current_channel[MEASUREMENT_UNITS_COUNT];
  unsigned int enabled_channels[MEASUREMENT_UNITS_COUNT];
  bool measurement_is_in_progress[MEASUREMENT_UNITS_COUNT];
  int channel_mappings[CHANNEL_TYPE_MAX+1][MULTIMETER_MAX_CHANNELS_PER_CHANNEL_TYPE];
  long long int measurement_results[MEASUREMENT_UNITS_COUNT][MULTIMETER_MAX_CHANNELS_PER_UNIT];
  unsigned int enabled_measurement_types;
  unsigned int tick_ms;

  int GetNextChannel(int unit) const;
public:
  explicit Multimeter(MeasurementUint **_units, unsigned int _enabled_measurement_types, unsigned int _tick_ms);
  void TimerEvent();
  unsigned int GetTicks(unsigned int ms) const;
  int GetMeasurementResult(MultimeterChannelType *channel_type, int *channel_no);
  int GetNumberOfChannels(MultimeterChannelType channel_type) const;
  void EnableChannels(unsigned int enabled_measurement_types);
};

#endif