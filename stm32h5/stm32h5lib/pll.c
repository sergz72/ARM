#include <board.h>
#include <pll.h>

void InitRCC(void)
{
  RCC->CR |= RCC_CR_HSEBYP;
  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY));

  //when VOS[1:0] = '0x11', the maximum value of fHCLK is 100 MHz.
  PWR->VOSCR |= PWR_VOSCR_VOS_0 | PWR_VOSCR_VOS_1;

  /* HCLK = SYSCLK */
  //RCC->CFGR2 |= RCC_CFGR2_HPRE_0;

  /* PCLK1 = HCLK */
  //RCC->CFGR2 |= RCC_CFGR2_PPRE1_2 | RCC_CFGR2_PPRE1_0;

  /* PCLK2 = HCLK */
  //RCC->CFGR2 |= RCC_CFGR2_PPRE2_2;

  /* PCLK3 = HCLK */
  //RCC->CFGR2 |= RCC_CFGR2_PPRE3_2;

  /* Configure the main PLL */
  RCC->PLL1CFGR = RCC_PLL1CFGR_PLL1PEN |
                  (PLL_M1 << 8) |
                  //PLL1VCOSEL is 0 - wide VCO range
                  // PLL1FRACEn is 0
                  RCC_PLL1CFGR_PLL1RGE_0 | // clock range frequency between 2 and 4 MHz
                  (RCC_PLL1CFGR_PLL1SRC_0 | RCC_PLL1CFGR_PLL1SRC_1); // HSE is the PLL source
  RCC->PLL1DIVR = (PLL_N1 - 1) |
                  ((PLL_P1 - 1) << 9);

  /* Enable the main PLL */
  RCC->CR |= RCC_CR_PLL1ON;

  /* Wait till the main PLL is ready */
  while((RCC->CR & RCC_CR_PLL1RDY) == 0);

  /* Configure Flash prefetch, ART Accelerator and wait state */
  FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_WRHIGHFREQ | FLASH_LATENCY;

  // enable icache
  ICACHE->CR = ICACHE_CR_CACHEINV;
  // waiting for full invalidate CACHEINV operation finished
  while (!(ICACHE->SR & ICACHE_SR_BSYENDF))
    ;
  ICACHE->CR = ICACHE_CR_EN;

  /* Select the main PLL as system clock source */
  RCC->CFGR1 &= (uint32_t)((uint32_t)~(RCC_CFGR1_SW));
  RCC->CFGR1 |= RCC_CFGR1_SW_0 | RCC_CFGR1_SW_1;

  /* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR1 & (uint32_t)RCC_CFGR1_SWS ) != (RCC_CFGR1_SWS_1 | RCC_CFGR1_SWS_0));
}
