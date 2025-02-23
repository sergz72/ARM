#ifndef _ADC_H
#define _ADC_H

#define VREFINT_VOLTAGE 1212 // mV

#define ADC_SampleTime_1_5Cycles   0
#define ADC_SampleTime_3_5Cycles   1
#define ADC_SampleTime_7_5Cycles   2
#define ADC_SampleTime_12_5Cycles  3
#define ADC_SampleTime_19_5Cycles  4
#define ADC_SampleTime_39_5Cycles  5
#define ADC_SampleTime_79_5Cycles  6
#define ADC_SampleTime_160_5Cycles 7

#define ADC_Enable() ADC1->CR |= ADC_CR_ADEN
#define ADC_Disable() ADC1->CR |= ADC_CR_ADDIS

void ADC_Init(int vbaten, unsigned int prescaler);

unsigned int ADC_GetValue(unsigned int channel, unsigned int sampling_time);

#endif
