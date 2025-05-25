#ifndef GENERATOR_H
#define GENERATOR_H

#define DEFAULT_TABLE_LENGTH 10240

typedef struct
{
  float sample_rate;
  float adder;
  float phase;
  int volume;
  short *table;
  float table_size;
} SignalGenerator;

void generator_init(SignalGenerator *generator, float sample_rate, short *table, int table_size);
short generator_calculate_next_value(SignalGenerator *generator);
void generator_set_frequency(SignalGenerator *generator, float frequency);
void generator_set_volume(SignalGenerator *generator, int volume);


#endif