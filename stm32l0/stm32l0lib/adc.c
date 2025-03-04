#include "board.h"
#include <adc.h>

unsigned int ADC_GetValue(unsigned int channel)
{
  unsigned int vref_value;

  //enable ADC
  ADC1->CR = ADC_CR_ADEN;

  ADC1->CHSELR = ADC_CHSELR_CHSEL17; // vrefint channel selected

  ADC1->CR |= ADC_CR_ADSTART;
  while (ADC1->CR & ADC_CR_ADSTART) {}
  vref_value = ADC1->DR;

  ADC1->CHSELR = 1 << channel;
  ADC1->CR |= ADC_CR_ADSTART;
  while (ADC1->CR & ADC_CR_ADSTART) {}

  //disable ADC
  ADC1->CR = 0;

  return ADC1->DR * VREFINT_VOLTAGE / vref_value;
}

void ADC_EnableIT(void)
{
}

void ADC_Start(void)
{
  //enable ADC
  ADC1->CR = ADC_CR_ADEN;
  ADC1->CR |= ADC_CR_ADSTART;
}

void ADC_WaitEOS(void)
{
  while (!(ADC1->ISR & ADC_ISR_EOS))
    __WFI();
}

void ADC_Disable(void)
{
  //disable ADC
  ADC1->CR = 0;
}
