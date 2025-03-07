#ifndef _PLL_H
#define _PLL_H

typedef struct
{
  unsigned int hse_frequency;
  unsigned int pllm;
  unsigned int hsebypass;
  unsigned int pllp_frequency;
  unsigned int pllq_frequency;
  unsigned int hpre;
  unsigned int ppre1;
  unsigned int ppre2;
  unsigned int ppre3;
} RCCConfig;

int InitRCC(const RCCConfig *config);

#endif
