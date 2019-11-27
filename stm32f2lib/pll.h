#ifndef _PLL_H
#define _PLL_H

void InitRCC(unsigned int oscill_freq, unsigned int hclk_freq, unsigned int usb_clock_freq, unsigned int latency,
             unsigned int APB2_Prescaler_value, unsigned int APB1_Prescaler_value);

#endif
