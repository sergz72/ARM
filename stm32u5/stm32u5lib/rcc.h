#ifndef _RCC_H
#define _RCC_H

typedef struct
{
  unsigned int m;
  unsigned int p_frequency;
  unsigned int q_frequency;
  unsigned int r_frequency;
} PLLConfig;

typedef struct
{
  PLLConfig pll[3];
  unsigned int hse_frequency;
  unsigned int hsebypass;
  unsigned int dpre;
  unsigned int hpre;
  unsigned int ppre1;
  unsigned int ppre2;
  unsigned int ppre3;
  bool apb2dis;
  bool apb1dis;
  bool ahb2dis1;
  bool ahb2dis2;
  bool ahb1dis;
  bool apb3dis;
  bool ahb3dis;
  bool vdd11usbdis;
  bool usbhsboosten;
  bool usbhspwren;
  bool boosten;
  bool flash_sleep_pd;
  bool flash_pdreq1;
  bool flash_pdreq2;
  unsigned int main_clock_source;
} RCCConfig;

int InitRCC(const RCCConfig *config);
int InitPLL(unsigned int hse_frequency, const PLLConfig *config, unsigned int pll_no);

#endif
