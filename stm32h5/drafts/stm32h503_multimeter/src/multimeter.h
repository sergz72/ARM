#ifndef STM32H503_MULTIMETER_MULTIMETER_H
#define STM32H503_MULTIMETER_MULTIMETER_H

#define CAPACITANCE_CHANNEL_1K   0
#define CAPACITANCE_CHANNEL_100K 1

typedef struct
{
  unsigned int pF;
  unsigned int diff;
  int channel;
} capacitance_result;

int get_capacitance(capacitance_result *result);
int multimeter_init(void);

#endif