#ifndef _ADC_H
#define _ADC_H

#define VREFINT_VOLTAGE 1224 // mV

unsigned int ADC_GetValue(unsigned int channel);
void ADC_Start(void);
void ADC_Disable(void);
void ADC_EnableIT(void);
void ADC_WaitEOS(void);

#endif
