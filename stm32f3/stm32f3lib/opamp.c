#include <board.h>
#include <opamp.h>

#define OPAMP_INPUT_INVERTING                 24 /*!< Inverting input */
#define OPAMP_INPUT_NONINVERTING              19 /*!< Non inverting input */

static void calibrate(OPAMP_TypeDef *amp, unsigned int offset, unsigned int input)
{
  unsigned int trimmingvalue = 16;
  unsigned int delta = 8;

  while (delta)
  {
    amp->CSR &= ~offset;
    amp->CSR |= trimmingvalue << input;
    delayms(2); // OFFTRIMmax delay 2 ms as per datasheet
    if (amp->CSR & OPAMP_CSR_OUTCAL)
        /* OPAMP_CSR_OUTCAL is HIGH try higher trimming */
        trimmingvalue += delta;
    else
        /* OPAMP_CSR_OUTCAL is LOW try lower trimming */
        trimmingvalue -= delta;
    delta >>= 1;
  }

  amp->CSR &= ~offset;
  amp->CSR |= trimmingvalue << input;
  delayms(2); // OFFTRIMmax delay 2 ms as per datasheet
  if (amp->CSR & OPAMP_CSR_OUTCAL)
  {
    trimmingvalue++;
    amp->CSR &= ~offset;
    amp->CSR |= trimmingvalue << input;
  }
}

void OPAMPCalibration(OPAMP_TypeDef *amp, unsigned int calseln, unsigned int calselp)
{
  unsigned int trimmingvaluen;
  unsigned int trimmingvaluep;
  unsigned int delta;

  // Non-inverting input connected to calibration reference voltage.
  // user trimming values are used for offset calibration
  // Enable calibration
  amp->CSR |= OPAMP_CSR_FORCEVP | OPAMP_CSR_USERTRIM | OPAMP_CSR_CALON;

  // 1st calibration - N, Select 3U% VREF
  amp->CSR &= ~OPAMP_CSR_CALSEL;
  amp->CSR |= calseln;

  //Enable the selected opamp
  amp->CSR |= OPAMP_CSR_OPAMPxEN;

  calibrate(amp, OPAMP_CSR_TRIMOFFSETN, OPAMP_INPUT_INVERTING);

  // 2nd calibration - P, Select 3U% VREF
  amp->CSR &= ~OPAMP_CSR_CALSEL;
  amp->CSR |= calselp;

  calibrate(amp, OPAMP_CSR_TRIMOFFSETP, OPAMP_INPUT_NONINVERTING);

  amp->CSR &= ~OPAMP_CSR_CALON;
  amp->CSR &= ~OPAMP_CSR_OPAMPxEN;
  amp->CSR &= ~OPAMP_CSR_FORCEVP;
}
