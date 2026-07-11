#ifndef _ADC_H
#define _ADC_H

#define VREFINT_VOLTAGE 1215 // mV

#define ADC_SampleTime_1_5Cycles   0
#define ADC_SampleTime_3_5Cycles   1
#define ADC_SampleTime_7_5Cycles   2
#define ADC_SampleTime_12_5Cycles  3
#define ADC_SampleTime_19_5Cycles  4
#define ADC_SampleTime_39_5Cycles  5
#define ADC_SampleTime_79_5Cycles  6
#define ADC_SampleTime_814_5Cycles 7

#define ADC_Disable(adc) adc->CR |= ADC_CR_ADDIS

void ADC_Init(ADC_TypeDef *adc, ADC_Common_TypeDef *adc_common, int vbaten, unsigned int prescaler);
void ADC_Calibrate(ADC_TypeDef *adc);
void ADC_Deinit(ADC_TypeDef *adc, ADC_Common_TypeDef *adc_common);
void ADC_Enable(ADC_TypeDef *adc);
unsigned int ADC_GetValue(ADC_TypeDef *adc, unsigned int channel, unsigned int sampling_time);

#endif
