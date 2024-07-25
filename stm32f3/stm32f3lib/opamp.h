#ifndef OPAMP_H
#define OPAMP_H

#define OPAMP_VREF_3VDDA                     0                      /*!< OPMAP Vref = 3.3U% VDDA */
#define OPAMP_VREF_10VDDA                    OPAMP_CSR_CALSEL_0     /*!< OPMAP Vref = 10U% VDDA  */
#define OPAMP_VREF_50VDDA                    OPAMP_CSR_CALSEL_1     /*!< OPMAP Vref = 50U% VDDA  */
#define OPAMP_VREF_90VDDA                    OPAMP_CSR_CALSEL       /*!< OPMAP Vref = 90U% VDDA  */

void OPAMPCalibration(OPAMP_TypeDef *amp, unsigned int calseln, unsigned int calselp);

#endif
