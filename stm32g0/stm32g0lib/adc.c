#include "board.h"
#include <adc.h>

void ADC_Init(int vbaten, unsigned int prescaler)
{
  RCC->APBENR2 |= RCC_APBENR2_ADCEN;

  unsigned int ccr = ADC_CCR_VREFEN | (((prescaler - 1) & 0x0F) << 18);
  if (vbaten)
    ccr |= ADC_CCR_VBATEN;
  ADC->CCR = ccr;

  ADC1->CR = ADC_CR_ADVREGEN;
  // wait 20 us
  delay(20);

  // calibration
  unsigned int sum = 0;
  for (int i = 0; i < 7; i++)
  {
    // start calibration
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL) // vait for calibration to complete
      ;
    sum += (ADC1->CALFACT & 0x7F) + 1;
  }
  sum >>= 3;
  if (sum > 0x7F)
    sum = 0x7F;
  ADC1->CALFACT = sum;
}

unsigned int ADC_GetValue(unsigned int channel, unsigned int sampling_time)
{
  unsigned int vref_value, value;

  ADC1->SMPR = sampling_time;

  ADC1->CHSELR = ADC_CHSELR_CHSEL13; // vrefint channel selected

  ADC1->CR |= ADC_CR_ADSTART;
  while (ADC1->CR & ADC_CR_ADSTART)
    ;
  vref_value = (unsigned short)ADC1->DR;

  ADC1->CHSELR = 1 << channel;
  ADC1->CR |= ADC_CR_ADSTART;
  while (ADC1->CR & ADC_CR_ADSTART)
    ;

  value = (unsigned short)ADC1->DR;

  return value * VREFINT_VOLTAGE / vref_value;
}
