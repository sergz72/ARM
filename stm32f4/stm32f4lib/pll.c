#include <board.h>
#include <pll.h>

void InitRCC(unsigned int oscill_freq, unsigned int hclk_freq, unsigned int latency,
             unsigned int AHB_Prescaler_value, unsigned int APB2_Prescaler_value, unsigned int APB1_Prescaler_value,
             unsigned int vos_value)
{
  unsigned int cfgr, pllm, plln, pllq, pllp, use_hse, freq2;

  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  cfgr = PWR->CR;
  cfgr &= ~PWR_CR_VOS;
  cfgr |= vos_value;
  PWR->CR = cfgr;

  use_hse = oscill_freq != 0;
  if (!use_hse)
    oscill_freq = 16; // HSI clock
  else
  {
    RCC->CR |= RCC_CR_HSEON;

    while (!(RCC->CR & RCC_CR_HSERDY));
  }

  cfgr = FLASH->ACR;
  cfgr |= FLASH_ACR_DCRST | FLASH_ACR_ICRST;

#ifndef DEBUG
  cfgr |= FLASH_ACR_PRFTEN | FLASH_ACR_DCEN | FLASH_ACR_ICEN;
#endif

  cfgr &= ~FLASH_ACR_LATENCY;
  cfgr |= latency;

  FLASH->ACR = cfgr;
 
/* HCLK = SYSCLK 															*/
/* AHB Prescaler = 1 														*/
  RCC->CFGR |= AHB_Prescaler_value;
 
/* APB2 Prescaler 														*/
  RCC->CFGR |= APB2_Prescaler_value;	   	
 
/* APB1 Prescaler 														*/
  RCC->CFGR |= APB1_Prescaler_value;		
 

  if (use_hse)
    cfgr = RCC_PLLCFGR_PLLSRC_HSE;
  else
    cfgr = RCC_PLLCFGR_PLLSRC_HSI;

  pllm = oscill_freq;
  if (pllm < 2 || pllm > 63)
    while (1); // wrong config

  freq2 = hclk_freq * 2;
  plln = freq2;
  pllp = 2;
  while (plln < 192)
  {
    plln += freq2;
    pllp += 2;
    if (plln > 432 || pllp > 8)
      while (1); // wrong config
  }

  pllp = (pllp >> 1) - 1;

  if (plln % 48)
    while (1); // wrong config
  pllq = plln / 48;
  if (pllq < 4 || pllq > 15)
    while (1); // wrong config

  cfgr |= pllm | (plln << 6U) | (pllp << 16U) | (pllq << 24U);
  RCC->PLLCFGR = cfgr;

  RCC->CR |= RCC_CR_PLLON;
 
  while(!(RCC->CR & RCC_CR_PLLRDY));
 
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
 
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

int InitPLL_I2S(unsigned int i2s_freq, unsigned int sai_freq)
{
  unsigned int start = i2s_freq * (100000000 / i2s_freq + 1);
  for (unsigned int f = start; f <= 432000000; f += i2s_freq)
  {
    if (!(f % 1000000) && !(f % sai_freq))
    {
      unsigned int n = f / 1000000;
      unsigned int r = f / i2s_freq;
      if (r < 2 || r > 7)
        return 2;
      unsigned int q = f / sai_freq;
      if (q < 2 || q > 15)
        return 3;
      RCC->PLLI2SCFGR = (n << 6) | (q << 24) | (r << 28);

      RCC->CR |= RCC_CR_PLLI2SON;

      while(!(RCC->CR & RCC_CR_PLLI2SRDY))
        ;

      return 0;
    }
  }

  return 1;
}