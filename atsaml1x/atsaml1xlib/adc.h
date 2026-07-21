#ifndef _ADC_H
#define _ADC_H

#define ADC_VREF_1V0  0
#define ADC_VREF_1V1  1
#define ADC_VREF_1V2  2
#define ADC_VREF_1V25 3
#define ADC_VREF_2V0  4
#define ADC_VREF_2V2  5
#define ADC_VREF_2V4  6
#define ADC_VREF_2V5  7

void adc_init(void);
unsigned int adc_get(unsigned short input); // with 0.1 mV resolution

#endif
