#ifndef _PLL_H
#define _PLL_H

void InitRCC(unsigned int oscill_freq, unsigned int hclk_freq, unsigned int latency,
             unsigned int AHB_Prescaler_value, unsigned int APB2_Prescaler_value, unsigned int APB1_Prescaler_value,
             unsigned int vos_value);
int InitPLL_I2S(unsigned int i2s_freq, unsigned int sai_freq);

#endif
