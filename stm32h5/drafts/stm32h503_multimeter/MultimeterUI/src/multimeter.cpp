#include "multimeter.h"
#include <cstring>

void MultimeterChannel::SetParameters(MeasurementUint* _measurement_unit, int _channel_no)
{
  measurement_unit = _measurement_unit;
  channel_no = _channel_no;
}

void MultimeterChannel::StartMeasurement()
{
  measurement_unit->StartMeasurement(channel_no, 0);
}

bool MultimeterChannel::IsMeasurementFinished()
{
  return measurement_unit->IsMeasurementFinished();
}

bool MultimeterChannel::IsReadyForNewMeasurement()
{
  return true;
}

Ampermeter::Ampermeter(long long int _R)
{
  R = _R;
}

MultimeterChannelType Ampermeter::GetChannelType()
{
  return CHANNEL_TYPE_CURRENT;
}

long long int Ampermeter::GetMeasurementResult()
{
  return measurement_unit->GetMeasurementResult() * 1000 / R;
}

Voltmeter::Voltmeter(long long int _coef)
{
  coef = _coef;
}

MultimeterChannelType Voltmeter::GetChannelType()
{
  return CHANNEL_TYPE_VOLTAGE;
}

long long int Voltmeter::GetMeasurementResult()
{
  return measurement_unit->GetMeasurementResult() * coef / 1000000;
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

long long int Ohmmeter::GetMeasurementResult()
{
  //todo
  return measurement_unit->GetMeasurementResult();
}

unsigned int MeasurementUint::GetTicks(unsigned int ms) const
{
  return multimeter->GetTicks(ms);
}

Multimeter::Multimeter(MeasurementUint **_units, unsigned int enabled_measurement_types, unsigned int _tick_ms)
{
  tick_ms = _tick_ms;
  measurement_units = _units;
  memset(current_channel, 0, sizeof(current_channel));
  memset(measurement_is_in_progress, false, sizeof(measurement_is_in_progress));
  memset(channel_mappings, -1, sizeof(channel_mappings));
  int indexes[CHANNEL_TYPE_MAX+1];
  memset(indexes, 0, sizeof(indexes));
  for (int unit = 0; unit < MEASUREMENT_UNITS_COUNT; unit++)
  {
    MeasurementUint *current_unit = measurement_units[unit];
    current_unit->SetParameters(this);
    int channels = current_unit->GetNumChannels();
    for (int channel = 0; channel <= channels; channel++)
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
  do
  {
    if (channel == measurement_units[unit]->GetNumChannels() - 1)
      channel = 0;
    else
      channel++;
  } while (!(enabled_measurement_types & (1 << measurement_units[unit]->GetChannel(channel)->GetChannelType())));
  return channel;
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
        measurement_results[i][current_channel[i]] = measurement_units[i]->GetChannel(channel)->GetMeasurementResult();
        measurement_is_in_progress[i] = false;
        current_channel[i] = GetNextChannel(i);
      }
      else
      {
        if (measurement_units[i]->GetChannel(channel)->IsReadyForNewMeasurement())
        {
          measurement_units[i]->GetChannel(channel)->StartMeasurement();
          measurement_is_in_progress[i] = true;
        }
      }
    }
  }
}

int Multimeter::GetMeasurementResult(MultimeterChannelType *channel_type, int *channel_no)
{
  //todo
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
