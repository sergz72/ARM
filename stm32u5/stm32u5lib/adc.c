#include "board.h"
#include <adc.h>

#define ADC_ISR_ALL_CLEAR 0x1B9F

void ADC_Init(ADC_TypeDef *adc, ADC_Common_TypeDef *adc_common, int vbaten, unsigned int prescaler)
{
  ADC_Deinit(adc, adc_common);

  unsigned int ccr = ADC_CCR_VREFEN | (((prescaler - 1) & 0x0F) << 18);
  if (vbaten)
    ccr |= ADC_CCR_VBATEN;
  adc_common->CCR = ccr;

  adc ->ISR = ADC_ISR_ALL_CLEAR;

  adc->CR = ADC_CR_ADVREGEN;
  while (!(adc->ISR & ADC_ISR_LDORDY))
    ;
}


void ADC_Calibrate(ADC_TypeDef *adc)
{
  // calibration
  unsigned int sum = 0;
  for (int i = 0; i < 7; i++)
  {
    // start calibration
    adc->CR |= ADC_CR_ADCAL;
    while (adc->CR & ADC_CR_ADCAL) // vait for calibration to complete
      ;
    sum += (adc->CALFACT & 0x7F) + 1;
  }
  sum >>= 3;
  if (sum > 0x7F)
    sum = 0x7F;
  adc->CR = ADC_CR_ADVREGEN | ADC_CR_ADEN;
  adc->CALFACT = sum;
  adc->CR = ADC_CR_ADVREGEN | ADC_CR_ADDIS;
}

void ADC_Deinit(ADC_TypeDef *adc, ADC_Common_TypeDef *adc_common)
{
  adc->CR |= ADC_CR_ADSTP;
  while (adc->CR & ADC_CR_ADSTP)
    ;
  adc_common->CCR = 0;
  adc->CR = ADC_CR_ADDIS;
  while (adc->CR & ADC_CR_ADDIS)
    ;
}

void ADC_Enable(ADC_TypeDef *adc)
{
  adc->ISR = ADC_ISR_ALL_CLEAR;
  adc->CR |= ADC_CR_ADEN;
  while (!(adc->ISR & ADC_ISR_ADRDY))
    ;
}

unsigned int ADC_GetValue(ADC_TypeDef *adc, unsigned int channel, unsigned int sampling_time)
{
  unsigned int vref_value, value;

  adc->SMPR1 = sampling_time;

  adc->CHSELR = ADC_CHSELR_CHSEL0; // vrefint channel selected

  adc->CR |= ADC_CR_ADSTART;
  while (adc->CR & ADC_CR_ADSTART)
    ;
  vref_value = adc->DR;

  adc->CHSELR = channel;
  adc->CR |= ADC_CR_ADSTART;
  while (adc->CR & ADC_CR_ADSTART)
    ;

  value = adc->DR;

  return value * VREFINT_VOLTAGE / vref_value;
}
