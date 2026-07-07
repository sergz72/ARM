#include <board.h>
#include <rcc.h>

static unsigned int calculate_flash_latency_vos2(const unsigned int hclk)
{
  return (hclk - 1) / 16000000;
}

static unsigned int calculate_flash_latency_vos1(const unsigned int hclk)
{
  return (hclk - 1) / 32000000;
}

int InitRCC(const RCCConfig *config)
{
  unsigned int rcc_cfgr2 = 0;

  RCC->CR = 0x11;
  RCC->CFGR1 = 0;

  // Enable Power Interface clock
  RCC->AHB1ENR2 |= RCC_AHB1ENR2_PWREN;

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

  unsigned int rcc_cfgr3 = 0;

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
      hclk_frequency = 12000000 / config->hpre;
      break;
    case 1: //hsi16
      hclk_frequency = 16000000 / config->hpre;
      break;
    case 2: // hse
      hclk_frequency = config->hse_frequency / config->hpre;
      break;
    default:
      return 5;
  }

  unsigned int flash_latency = 0;
  unsigned int vosr = 0;
  if (config->boosten)
    vosr |= PWR_VOSR_BOOSTEN;
  //The regulator operates in the following ranges:
  //Range 1 (VCORE = 0.9 V) with CPU and peripherals running at up to 96 MHz
  //Range 2 (VCORE = 0.75 V) with CPU and peripherals running at up to 48 MHz
  if (hclk_frequency > 48000000)
  {
    vosr |= PWR_VOSR_R2EN;
    PWR->VOSR = vosr;
    while (!(PWR->VOSR & PWR_VOSR_R2RDY))
      ;
    flash_latency = calculate_flash_latency_vos2(hclk_frequency);
  }
  else
  {
    vosr |= PWR_VOSR_R1EN;
    PWR->VOSR = vosr;
    while (!(PWR->VOSR & PWR_VOSR_R1RDY))
      ;
    flash_latency = calculate_flash_latency_vos1(hclk_frequency);
  }

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

  // enable icache
  ICACHE->CR = ICACHE_CR_CACHEINV;
  // waiting for full invalidate CACHEINV operation finished
  while (!(ICACHE->SR & ICACHE_SR_BSYENDF))
    ;
  ICACHE->CR = ICACHE_CR_EN;

  /* Select system clock source */
  RCC->CFGR1 |= config->main_clock_source;
  /* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR1 & RCC_CFGR1_SWS) != config->main_clock_source << 2)
    ;

  return 0;
}
