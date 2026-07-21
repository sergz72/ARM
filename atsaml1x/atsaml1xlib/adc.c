#include <adc.h>
#include "board.h"

#define NVM_SOFTWARE_CALIBRATION_ROW_ADDR 0x00806020

void adc_init(void)
{
  unsigned int calibration_value = *((unsigned int*)NVM_SOFTWARE_CALIBRATION_ROW_ADDR);
  unsigned int adc_biascomp = (calibration_value >> 3) & 7;
  unsigned int adc_biasrefbuf = calibration_value & 7;

  SUPC_REGS->SUPC_VREF = SUPC_VREF_VREFOE(1) | ADC_REFERENCE;
  /* Write linearity calibration and bias calibration */
  ADC_REGS->ADC_CALIB = adc_biascomp | (adc_biasrefbuf << 8);

  /* Sampling length */
  ADC_REGS->ADC_SAMPCTRL = ADC_SAMPCTRL_OFFCOMP(1);
  while(ADC_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_SAMPCTRL_Msk)
  {
    /* Wait for Synchronization */
  }

  ADC_REGS->ADC_REFCTRL = ADC_REFCTRL_REFCOMP(1);

  /* Prescaler, Resolution & Operation Mode */
  ADC_REGS->ADC_CTRLB = ADC_PRESCALER;

  ADC_REGS->ADC_CTRLB = ADC_CTRLC;
  while(ADC_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_CTRLC_Msk)
  {
    /* Wait for Synchronization */
  }

  /* Clear all interrupt flags */
  ADC_REGS->ADC_INTFLAG = ADC_INTFLAG_Msk;

  ADC_REGS->ADC_CTRLA = ADC_CTRLA_ENABLE_Msk;
  while(ADC_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_ENABLE_Msk)
  {
    /* Wait for Synchronization */
  }
}

#if ADC_REFERENCE == 0
#define ADC_VREF 10000
#elif ADC_REFERENCE == 1
#define ADC_VREF 11000
#elif ADC_REFERENCE == 2
#define ADC_VREF 12000
#elif ADC_REFERENCE == 3
#define ADC_VREF 12500
#elif ADC_REFERENCE == 4
#define ADC_VREF 20000
#elif ADC_REFERENCE == 5
#define ADC_VREF 22000
#elif ADC_REFERENCE == 6
#define ADC_VREF 24000
#else
#define ADC_VREF 25000
#endif

unsigned int adc_get(unsigned short input)
{
  ADC_REGS->ADC_INPUTCTRL = input;
  while(ADC_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_INPUTCTRL_Msk)
  {
    /* Wait for Synchronization */
  }

  ADC_REGS->ADC_INTFLAG = ADC_INTFLAG_RESRDY_Msk;
  /* Start conversion */
  ADC_REGS->ADC_SWTRIG = ADC_SWTRIG_START_Msk;
  while (!(ADC_REGS->ADC_INTFLAG & ADC_INTFLAG_RESRDY_Msk))
    ;
  unsigned int result = ADC_REGS->ADC_RESULT;
  return (result * ADC_VREF) >> 12;
}
