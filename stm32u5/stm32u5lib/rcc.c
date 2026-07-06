#include <board.h>
#include <rcc.h>

static unsigned int calculate_flash_latency_vos4(unsigned int hclk)
{
  return hclk <= 12000000 ? 0 : 1;
}

static unsigned int calculate_flash_latency_vos3(unsigned int hclk)
{
  return (hclk - 1) / 24000000;
}

static unsigned int calculate_flash_latency_vos2(unsigned int hclk)
{
  return (hclk - 1) / 30000000;
}

static unsigned int calculate_flash_latency_vos1(unsigned int hclk)
{
  return (hclk - 1) / 32000000;
}

static bool is_pll_active(const PLLConfig *config)
{
  return config->p_frequency != 0 || config->q_frequency != 0 || config->r_frequency != 0;
}

int InitPLL(unsigned int hse_frequency, const PLLConfig *config, unsigned int pll_no)
{
  if (!is_pll_active(config))
    return 0;

  unsigned int vos = (PWR->VOSR >> 16) & 3;
  unsigned int pll_min_frequency = 128000000;
  unsigned int pll_max_frequency = 0;
  switch (vos)
  {
    case 0: // range 4
      return 1;
    case 1:
      pll_max_frequency = 330000000;
      break;
    case 2:
    case 3:
      pll_max_frequency = 544000000;
      break;
  }

  unsigned int refclk = hse_frequency / config->m;
  if (refclk < 4000000 || refclk > 16000000)
    return 2;

  unsigned int plln = pll_min_frequency / refclk;
  if (pll_min_frequency % refclk)
    plln++;
  unsigned int pll_frequency = refclk * plln;
  unsigned int pllp = 1;
  unsigned int pllq = 1;
  unsigned int pllr = 1;
  while (pll_frequency <= pll_max_frequency)
  {
    if ((!config->p_frequency || !(pll_frequency % config->p_frequency))
      && (!config->q_frequency || !(pll_frequency % config->q_frequency))
      && (!config->r_frequency || !(pll_frequency % config->r_frequency)))
    {
      if (config->r_frequency)
        pllr = pll_frequency / config->r_frequency;
      if (config->q_frequency)
        pllq = pll_frequency / config->q_frequency;
      if (config->p_frequency)
        pllp = pll_frequency / config->p_frequency;
      if (pllr != 3)
        break;
    }
    pll_frequency += refclk;
    plln++;
  }
  if (pll_frequency > pll_max_frequency)
    return 3;

  unsigned int pll_input_frequency_range = refclk < 8000000 ? 0 : 0x0C;

  /* Configure the main PLL */
  unsigned int temp = RCC_PLL1CFGR_PLL1PEN |
                  ((config->m - 1) << 8) |
                  pll_input_frequency_range |
                  (RCC_PLL1CFGR_PLL1SRC_0 | RCC_PLL1CFGR_PLL1SRC_1); // HSE is the PLL source
  if (config->p_frequency)
    temp |= RCC_PLL1CFGR_PLL1PEN;
  if (config->q_frequency)
    temp |= RCC_PLL1CFGR_PLL1QEN;
  if (config->r_frequency)
    temp |= RCC_PLL1CFGR_PLL1REN;
  unsigned int divr = (plln - 1) | ((pllp - 1) << 9) | ((pllq - 1) << 16) | ((pllr - 1) << 24);
  switch (pll_no)
  {
    case 0:
      RCC->PLL1CFGR = temp;
      RCC->PLL1DIVR = divr;

      /* Enable the main PLL */
      RCC->CR |= RCC_CR_PLL1ON;
      /* Wait till the main PLL is ready */
      while((RCC->CR & RCC_CR_PLL1RDY) == 0)
        ;
      break;
    case 1:
      RCC->PLL2CFGR = temp;
      RCC->PLL2DIVR = divr;

      /* Enable the main PLL */
      RCC->CR |= RCC_CR_PLL2ON;
      /* Wait till the main PLL is ready */
      while((RCC->CR & RCC_CR_PLL2RDY) == 0)
        ;
      break;
    case 2:
      RCC->PLL3CFGR = temp;
      RCC->PLL3DIVR = divr;

      /* Enable the main PLL */
      RCC->CR |= RCC_CR_PLL3ON;
      /* Wait till the main PLL is ready */
      while((RCC->CR & RCC_CR_PLL3RDY) == 0)
        ;
      break;
    default:
      return 4;
  }

  return 0;
}

int InitRCC(const RCCConfig *config)
{
  unsigned int rcc_cfgr2 = 0;

  RCC->CR = 0x11;
  RCC->CFGR1 = 0;

  // 1. Enable Power Interface clock
  RCC->AHB3ENR |= RCC_AHB3ENR_PWREN;
  
  if (config->ahb1dis)
    rcc_cfgr2 |= RCC_CFGR2_AHB1DIS;
  if (config->ahb2dis1)
    rcc_cfgr2 |= RCC_CFGR2_AHB2DIS1;
  if (config->ahb2dis2)
    rcc_cfgr2 |= RCC_CFGR2_AHB2DIS2;
  if (config->apb1dis)
    rcc_cfgr2 |= RCC_CFGR2_APB1DIS;
  if (config->apb2dis)
    rcc_cfgr2 |= RCC_CFGR2_APB2DIS;

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

  switch (config->dpre)
  {
  case 1:
    break;
  case 2:
    rcc_cfgr2 |= 4 << 12;
    break;
  case 4:
    rcc_cfgr2 |= 5 << 12;
    break;
  case 8:
    rcc_cfgr2 |= 6 << 12;
    break;
  case 16:
    rcc_cfgr2 |= 7 << 12;
    break;
  default:
    return 3;
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

  unsigned int rcc_cfgr3 = 0;

  if (config->apb3dis)
    rcc_cfgr3 |= RCC_CFGR3_APB3DIS;
  if (config->ahb3dis)
    rcc_cfgr3 |= RCC_CFGR3_AHB3DIS;

  switch (config->ppre3)
  {
    case 1:
      break;
    case 2:
      rcc_cfgr3 |= RCC_CFGR3_PPRE3_2;
      break;
    case 4:
      rcc_cfgr3 |= RCC_CFGR3_PPRE3_2 | RCC_CFGR3_PPRE3_0;
      break;
    case 8:
      rcc_cfgr3 |= RCC_CFGR3_PPRE3_2 | RCC_CFGR3_PPRE3_1;
      break;
    case 16:
      rcc_cfgr3 |= RCC_CFGR3_PPRE3_2 | RCC_CFGR3_PPRE3_1 | RCC_CFGR3_PPRE3_0;
      break;
    default:
      return 4;
  }

  if (config->main_clock_source >= 2)
  {
    if (config->hsebypass)
      RCC->CR |= RCC_CR_HSEBYP | RCC_CR_HSEEXT;
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY))
      ;
  }

  unsigned int hclk_frequency;
  switch (config->main_clock_source)
  {
    case 0: //msis
      hclk_frequency = 4000000 / config->hpre;
      break;
    case 1: //hsi16
      hclk_frequency = 16000000 / config->hpre;
      break;
    case 2: // hse
      hclk_frequency = config->hse_frequency / config->hpre;
      break;
    case 3:
      hclk_frequency = config->pll[0].r_frequency / config->hpre;
      break;
    default:
      return 5;
  }

  unsigned int flash_latency = 0;
  unsigned int vosr = 0;
  if (config->vdd11usbdis)
    vosr |= 1 << 21;
  if (config->usbhsboosten)
    vosr |= 1 << 20;
  if (config->usbhspwren)
    vosr |= 1 << 19;
  if (config->boosten)
    vosr |= PWR_VOSR_BOOSTEN;
  // Voltage range 1 for 110 MHz < fHCLK ≤ 160 MHz
  // Voltage range 2 for 55 MHz < fHCLK ≤ 110 MHz
  // Voltage range 3 for 25 MHz < fHCLK ≤ 55 MHz
  // Voltage range 4 for fHCLK ≤ 25 MHz
  if (hclk_frequency > 25000000)
  {
    if (hclk_frequency <= 55000000)
    {
      vosr |= PWR_VOSR_VOS_0;
      flash_latency = calculate_flash_latency_vos3(hclk_frequency);
    }
    else if (hclk_frequency <= 110000000)
    {
      vosr |= PWR_VOSR_VOS_1;
      flash_latency = calculate_flash_latency_vos2(hclk_frequency);
    }
    else
    {
      vosr |= PWR_VOSR_VOS_0 | PWR_VOSR_VOS_1;
      flash_latency = calculate_flash_latency_vos1(hclk_frequency);
    }
    PWR->VOSR = vosr;
  }
  else
    flash_latency = calculate_flash_latency_vos4(hclk_frequency);

  RCC->CFGR2 = rcc_cfgr2;
  RCC->CFGR3 = rcc_cfgr3;

  unsigned int flash_acr = flash_latency;
  if (flash_latency)
    flash_acr |= FLASH_ACR_PRFTEN;
  if (config->flash_sleep_pd)
    flash_acr |= FLASH_ACR_SLEEP_PD;
  /* Configure Flash prefetch and wait state */
  FLASH->ACR = flash_acr;
  if (config->flash_pdreq1)
  {
    FLASH->PDKEY1R = 0x04152637;
    FLASH->PDKEY1R = 0xFAFBFCFD;
    FLASH->ACR |= FLASH_ACR_PDREQ1;
  }
  if (config->flash_pdreq2)
  {
    FLASH->PDKEY2R = 0x40516273;
    FLASH->PDKEY2R = 0xAFBFCFDF;
    FLASH->ACR |= FLASH_ACR_PDREQ2;
  }

  int rc = InitPLL(config->hse_frequency, &config->pll[0], 0);
  if (rc)
    return rc;
  rc = InitPLL(config->hse_frequency, &config->pll[1], 1);
  if (rc)
    return rc;
  rc = InitPLL(config->hse_frequency, &config->pll[2], 2);
  if (rc)
    return rc;

  // enable icache
  ICACHE->CR = ICACHE_CR_CACHEINV;
  // waiting for full invalidate CACHEINV operation finished
  while (!(ICACHE->SR & ICACHE_SR_BSYENDF))
    ;
  ICACHE->CR = ICACHE_CR_EN;

  /* Select system clock source */
  RCC->CFGR1 |= config->main_clock_source;
  /* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR1 & RCC_CFGR1_SWS) != (config->main_clock_source << 2))
    ;

  return 0;
}
