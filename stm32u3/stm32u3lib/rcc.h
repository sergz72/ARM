#ifndef _RCC_H
#define _RCC_H

typedef struct
{
  unsigned int hse_frequency;
  unsigned int hsebypass;
  unsigned int hpre;
  unsigned int ppre1;
  unsigned int ppre2;
  unsigned int ppre3;
  bool boosten;
  bool flash_sleep_pd;
  bool flash_pdreq1;
  bool flash_pdreq2;
  unsigned int main_clock_source;
} RCCConfig;

int InitRCC(const RCCConfig *config);

#endif
