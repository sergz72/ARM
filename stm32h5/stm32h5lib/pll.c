#include <board.h>
#include <pll.h>

static unsigned int calculate_flash_latency_vos3(unsigned int hclk)
{
  return (hclk - 1) / 20000000;
}

static unsigned int calculate_flash_latency_vos2(unsigned int hclk)
{
  return (hclk - 1) / 30000000;
}

static unsigned int calculate_flash_latency_vos1(unsigned int hclk)
{
  return (hclk - 1) / 34000000;
}

static unsigned int calculate_flash_latency_vos0(unsigned int hclk)
{
  return (hclk - 1) / 42000000;
}

int InitRCC(const RCCConfig *config)
{
  unsigned int pll_min_frequency, pll_max_frequency;

  unsigned int refclk = config->hse_frequency / config->pllm;
  if (refclk < 1000000 || refclk > 16000000)
    return 1;

  unsigned int rcc_cfgr2 = 0;
  switch (config->hpre)
  {
    case 1:
      break;
    case 2:
      rcc_cfgr2 |= RCC_CFGR2_HPRE_3;
      break;
    case 4:
      rcc_cfgr2 |= RCC_CFGR2_HPRE_3 | RCC_CFGR2_HPRE_0;
      break;
    case 8:
      rcc_cfgr2 |= RCC_CFGR2_HPRE_3 | RCC_CFGR2_HPRE_1;
      break;
    case 16:
      rcc_cfgr2 |= RCC_CFGR2_HPRE_3 | RCC_CFGR2_HPRE_0 | RCC_CFGR2_HPRE_1;
      break;
    case 64:
      rcc_cfgr2 |= RCC_CFGR2_HPRE_3 | RCC_CFGR2_HPRE_2;
      break;
    case 128:
      rcc_cfgr2 |= RCC_CFGR2_HPRE_3 | RCC_CFGR2_HPRE_2 | RCC_CFGR2_HPRE_0;
      break;
    case 256:
      rcc_cfgr2 |= RCC_CFGR2_HPRE_3 | RCC_CFGR2_HPRE_2 | RCC_CFGR2_HPRE_1;
      break;
    case 512:
      rcc_cfgr2 |= RCC_CFGR2_HPRE_3 | RCC_CFGR2_HPRE_2 | RCC_CFGR2_HPRE_1 | RCC_CFGR2_HPRE_0;
      break;
    default:
      return 2;
  }
  switch (config->ppre1)
  {
    case 1:
      break;
    case 2:
      rcc_cfgr2 |= RCC_CFGR2_PPRE1_2;
      break;
    case 4:
      rcc_cfgr2 |= RCC_CFGR2_PPRE1_2 | RCC_CFGR2_PPRE1_0;
      break;
    case 8:
      rcc_cfgr2 |= RCC_CFGR2_PPRE1_2 | RCC_CFGR2_PPRE1_1;
      break;
    case 16:
      rcc_cfgr2 |= RCC_CFGR2_PPRE1_2 | RCC_CFGR2_PPRE1_1 | RCC_CFGR2_PPRE1_0;
      break;
    default:
      return 3;
  }

  switch (config->ppre2)
  {
    case 1:
      break;
    case 2:
      rcc_cfgr2 |= RCC_CFGR2_PPRE2_2;
      break;
    case 4:
      rcc_cfgr2 |= RCC_CFGR2_PPRE2_2 | RCC_CFGR2_PPRE2_0;
      break;
    case 8:
      rcc_cfgr2 |= RCC_CFGR2_PPRE2_2 | RCC_CFGR2_PPRE2_1;
      break;
    case 16:
      rcc_cfgr2 |= RCC_CFGR2_PPRE2_2 | RCC_CFGR2_PPRE2_1 | RCC_CFGR2_PPRE2_0;
      break;
    default:
      return 3;
  }

  switch (config->ppre3)
  {
    case 1:
      break;
    case 2:
      rcc_cfgr2 |= RCC_CFGR2_PPRE3_2;
      break;
    case 4:
      rcc_cfgr2 |= RCC_CFGR2_PPRE3_2 | RCC_CFGR2_PPRE3_0;
      break;
    case 8:
      rcc_cfgr2 |= RCC_CFGR2_PPRE3_2 | RCC_CFGR2_PPRE3_1;
      break;
    case 16:
      rcc_cfgr2 |= RCC_CFGR2_PPRE3_2 | RCC_CFGR2_PPRE3_1 | RCC_CFGR2_PPRE3_0;
      break;
    default:
      return 4;
  }

  unsigned int medium_vco_frequency_range = refclk < 2000000 ? RCC_PLL1CFGR_PLL1VCOSEL : 0;

  if (medium_vco_frequency_range)
  {
    pll_min_frequency = 150000000;
    pll_max_frequency = 420000000;
  }
  else
  {
    pll_min_frequency = 128000000;
    pll_max_frequency = 560000000;
  }

  unsigned int plln = pll_min_frequency / refclk + 1;
  unsigned int pll_frequency = refclk * plln;
  unsigned int pllp = 0;
  unsigned int pllq = 1;
  while (pll_frequency <= pll_max_frequency)
  {
    if (!(pll_frequency % config->pllp_frequency) && (!config->pllq_frequency || !(pll_frequency % config->pllq_frequency)))
    {
      if (config->pllq_frequency)
        pllq = pll_frequency / config->pllq_frequency;
      pllp = pll_frequency / config->pllp_frequency;
      if (pllp != 1 && pllp != 3)
        break;
    }
    pll_frequency += refclk;
    plln++;
  }
  if (pll_frequency > pll_max_frequency)
    return 2;

  if (config->hsebypass)
    RCC->CR |= RCC_CR_HSEBYP;
  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;

  unsigned int hclk_frequency = config->pllp_frequency / config->hpre;

  unsigned int flash_latency = 0;
  unsigned int temp;
  if (hclk_frequency > 100000000)
  {
    temp = PWR->VOSCR;
    temp &= ~PWR_VOSCR_VOS_Msk;
    if (hclk_frequency <= 150000000)
    {
      temp |= PWR_VOSCR_VOS_0;
      flash_latency = calculate_flash_latency_vos2(hclk_frequency);
    }
    else if (hclk_frequency <= 200000000)
    {
      temp |= PWR_VOSCR_VOS_1;
      flash_latency = calculate_flash_latency_vos1(hclk_frequency);
    }
    else
    {
      temp |= PWR_VOSCR_VOS_0 | PWR_VOSCR_VOS_1;
      flash_latency = calculate_flash_latency_vos0(hclk_frequency);
    }
    PWR->VOSCR = temp;
  }
  else
    flash_latency = calculate_flash_latency_vos3(hclk_frequency);

  RCC->CFGR2 = rcc_cfgr2;

  unsigned int pll_clock_range = 0;
  if (refclk < 2000000)
    pll_clock_range = 0;
  else if (refclk < 4000000)
    pll_clock_range = RCC_PLL1CFGR_PLL1RGE_0;
  else if (refclk < 8000000)
    pll_clock_range = RCC_PLL1CFGR_PLL1RGE_1;
  else
    pll_clock_range = RCC_PLL1CFGR_PLL1RGE_0 | RCC_PLL1CFGR_PLL1RGE_1;

  /* Configure the main PLL */
  temp = RCC_PLL1CFGR_PLL1PEN |
                  ((config->pllm == 1 ? 0 : config->pllm) << 8) |
                  medium_vco_frequency_range |
                  // PLL1FRACEn is 0
                  pll_clock_range | // clock range frequency between 2 and 4 MHz
                  (RCC_PLL1CFGR_PLL1SRC_0 | RCC_PLL1CFGR_PLL1SRC_1); // HSE is the PLL source
  if (config->pllq_frequency)
    temp |= RCC_PLL1CFGR_PLL1QEN;
  RCC->PLL1CFGR = temp;
  RCC->PLL1DIVR = (plln - 1) | ((pllp - 1) << 9) | ((pllq - 1) << 16);

  /* Enable the main PLL */
  RCC->CR |= RCC_CR_PLL1ON;
  /* Wait till the main PLL is ready */
  while((RCC->CR & RCC_CR_PLL1RDY) == 0)
    ;

  unsigned int wrhighfreq = (flash_latency / 2) << FLASH_ACR_WRHIGHFREQ_Pos;
  /* Configure Flash prefetch, ART Accelerator and wait state */
  FLASH->ACR = FLASH_ACR_PRFTEN | wrhighfreq | flash_latency;

  // enable icache
  ICACHE->CR = ICACHE_CR_CACHEINV;
  // waiting for full invalidate CACHEINV operation finished
  while (!(ICACHE->SR & ICACHE_SR_BSYENDF))
    ;
  ICACHE->CR = ICACHE_CR_EN;

  /* Select the main PLL as system clock source */
  RCC->CFGR1 |= RCC_CFGR1_SW_0 | RCC_CFGR1_SW_1;
  /* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR1 & RCC_CFGR1_SWS) != (RCC_CFGR1_SWS_1 | RCC_CFGR1_SWS_0))
    ;

  return 0;
}
