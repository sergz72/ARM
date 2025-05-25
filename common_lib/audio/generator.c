#include "generator.h"

void generator_init(SignalGenerator *generator, float sample_rate, short *table, int table_size)
{
  generator->sample_rate = sample_rate;
  generator->volume = 0xFFFF;
  generator->table = table;
  generator->table_size = (float)table_size;
  generator->phase = 0;
  generator->adder = 0;
}

short generator_calculate_next_value(SignalGenerator *generator)
{
  short v = (short)((generator->table[(int)generator->phase] * generator->volume) >> 16);
  generator->phase += generator->adder;
  if (generator->phase >= generator->table_size)
    generator->phase -= generator->table_size;
  return v;
}

void generator_set_frequency(SignalGenerator *generator, float frequency)
{
  generator->adder = generator->table_size * frequency / generator->sample_rate;
}

void generator_set_volume(SignalGenerator *generator, int volume)
{
  generator->volume = volume;
}