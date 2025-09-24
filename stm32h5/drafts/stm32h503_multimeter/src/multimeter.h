#ifndef STM32H503_MULTIMETER_MULTIMETER_H
#define STM32H503_MULTIMETER_MULTIMETER_H

int get_capacitance(unsigned int *pF, unsigned int *diff);
void multimeter_init(void);

#endif