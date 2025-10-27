#include "multimeter.h"

#include <cmath>
#include <cstring>
#include <limits.h>

void MultimeterChannel::SetParameters(MeasurementUint* _measurement_unit, int _channel_no)
{
  measurement_unit = _measurement_unit;
  channel_no = _channel_no;
}

void MultimeterChannel::StartMeasurement()
{
  measurement_unit->StartMeasurement(channel_no);
}

bool MultimeterChannel::IsMeasurementFinished()
{
  return measurement_unit->IsMeasurementFinished();
}

bool MultimeterChannel::IsReadyForNewMeasurement()
{
  return true;
}

bool Meter::IsMeasurementFinished()
{
  bool finished = measurement_unit->IsMeasurementFinished();
  if (finished)
  {
    result = measurement_unit->GetMeasurementResult();
    if (result == INT_MAX || result == INT_MIN)
    {
      if (current_gain != 1)
      {
        current_gain = 1;
        measurement_unit->SetGain(channel_no, 1);
        measurement_unit->StartMeasurement(channel_no);
        return false;
      }
      return true;
    }
    int gain = result == 0 ? INT_MAX : measurement_unit->GetMaxValue(channel_no) * current_gain / abs(result);
    gain = measurement_unit->SetGain(channel_no, gain);
    if (gain != current_gain)
    {
      measurement_unit->StartMeasurement(channel_no);
      current_gain = gain;
      return false;
    }
    return true;
  }
  return false;
}

Ampermeter::Ampermeter(long long int _R)
{
  multiplier = _R;
}

void Ampermeter::SetParameters(MeasurementUint *_measurement_unit, int _channel_no)
{
  Meter::SetParameters(_measurement_unit, _channel_no);
  multiplier = static_cast<long long int>(measurement_unit->GetVref()) * 1000000000LL / multiplier;
}

MultimeterChannelType Ampermeter::GetChannelType()
{
  return CHANNEL_TYPE_CURRENT;
}

int Ampermeter::GetMeasurementResult()
{
  if (result == INT_MAX)
    return INT_MAX;
  if (result == INT_MIN)
    return INT_MIN;
  return static_cast<int>(static_cast<long long int>(result) * multiplier /
              measurement_unit->GetMaxValue(channel_no) / current_gain);
}

Voltmeter::Voltmeter(long long int _coef)
{
  coef = _coef;
}

MultimeterChannelType Voltmeter::GetChannelType()
{
  return CHANNEL_TYPE_VOLTAGE;
}

int Voltmeter::GetMeasurementResult()
{
  if (result == INT_MAX)
    return INT_MAX;
  if (result == INT_MIN)
    return INT_MIN;
  return static_cast<int>(static_cast<long long int>(result) * measurement_unit->GetVref() * coef * 1000 /
                            measurement_unit->GetMaxValue(channel_no) / current_gain);
}

Ohmmeter::Ohmmeter()
{
  current_level = CURRENT_LEVEL_LO;
}

MultimeterChannelType Ohmmeter::GetChannelType()
{
  return CHANNEL_TYPE_RESISTANCE;
}

bool Ohmmeter::IsMeasurementFinished()
{
  //todo
  return measurement_unit->IsMeasurementFinished();
}

int Ohmmeter::GetMeasurementResult()
{
  //todo
  return measurement_unit->GetMeasurementResult();
}

unsigned int MeasurementUint::GetTicks(unsigned int ms) const
{
  return multimeter->GetTicks(ms);
}

int MeasurementUint::SetGain(int channel, int gain)
{
  return 1;
}


Multimeter::Multimeter(MeasurementUint **_units, unsigned int enabled_measurement_types, unsigned int _tick_ms)
{
  tick_ms = _tick_ms;
  measurement_units = _units;
  memset(current_channel, 0, sizeof(current_channel));
  memset(measurement_is_in_progress, false, sizeof(measurement_is_in_progress));
  memset(channel_mappings, -1, sizeof(channel_mappings));
  memset(measurement_results, 0, sizeof(measurement_results));
  int indexes[CHANNEL_TYPE_MAX+1];
  memset(indexes, 0, sizeof(indexes));
  for (int unit = 0; unit < MEASUREMENT_UNITS_COUNT; unit++)
  {
    MeasurementUint *current_unit = measurement_units[unit];
    current_unit->SetParameters(this);
    int channels = current_unit->GetNumChannels();
    for (int channel = 0; channel < channels; channel++)
    {
      MultimeterChannel *c = current_unit->GetChannel(channel);
      MultimeterChannelType t = c->GetChannelType();
      channel_mappings[t][indexes[t]] = (unit << 16) | channel;
      indexes[t]++;
    }
  }
  EnableChannels(enabled_measurement_types);
}

void Multimeter::EnableChannels(unsigned int _enabled_measurement_types)
{
  enabled_measurement_types = _enabled_measurement_types;
}

int Multimeter::GetNextChannel(int unit) const
{
  int channel = current_channel[unit];
  int n = measurement_units[unit]->GetNumChannels();
  while (n--)
  {
    if (channel == measurement_units[unit]->GetNumChannels() - 1)
      channel = 0;
    else
      channel++;
    if (enabled_measurement_types & (1 << measurement_units[unit]->GetChannel(channel)->GetChannelType()))
      return channel;
  };
  return -1;
}

void Multimeter::TimerEvent()
{
  for (int i = 0; i < MEASUREMENT_UNITS_COUNT; i++)
  {
    int channel = current_channel[i];
    if (measurement_is_in_progress[i])
    {
      if (measurement_units[i]->GetChannel(channel)->IsMeasurementFinished())
      {
        MeasurementResult *result = &measurement_results[i][channel];
        result->value = measurement_units[i]->GetChannel(channel)->GetMeasurementResult();
        result->done = true;
        measurement_is_in_progress[i] = false;
      }
    }
    else
    {
      channel = GetNextChannel(i);
      if (channel == -1)
        continue;
      current_channel[i] = channel;
      if (measurement_units[i]->GetChannel(channel)->IsReadyForNewMeasurement())
      {
        measurement_units[i]->GetChannel(channel)->StartMeasurement();
        measurement_is_in_progress[i] = true;
      }
    }
  }
}

int Multimeter::GetMeasurementResult(MultimeterChannelType *channel_type, int *channel_no)
{
  for (int unit = 0; unit < MEASUREMENT_UNITS_COUNT; unit++)
  {
    for (int channel = 0; channel < MULTIMETER_MAX_CHANNELS_PER_UNIT; channel++)
    {
      MeasurementResult *result = &measurement_results[unit][channel];
      if (result->done)
      {
        MultimeterChannelType ctype = measurement_units[unit]->GetChannel(channel)->GetChannelType();
        *channel_type = ctype;
        for (int ch = 0; ch < MULTIMETER_MAX_CHANNELS_PER_CHANNEL_TYPE; ch++)
        {
          int mapping = channel_mappings[ctype][ch];
          if (mapping == ((unit << 16) | channel))
          {
            *channel_no = ch;
            result->done = false;
            return result->value;
          }
        }
        goto exit;
      }
    }
  }
exit:
  *channel_type = CHANNEL_TYPE_NONE;
  return 0;
}

int Multimeter::GetNumberOfChannels(MultimeterChannelType channel_type) const
{
  int result = 0;
  for (int i = 0; i < MULTIMETER_MAX_CHANNELS_PER_CHANNEL_TYPE; i++)
  {
    if (channel_mappings[channel_type][i] == -1)
      break;
    result++;
  }
  return result;
}

unsigned int Multimeter::GetTicks(unsigned int ms) const
{
  unsigned int result = ms / tick_ms;
  return ms % tick_ms ? result + 1 : result;
}
