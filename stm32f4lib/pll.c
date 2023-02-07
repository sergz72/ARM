#include <board.h>
#include <pll.h>

void InitRCC(unsigned int oscill_freq, unsigned int hclk_freq, unsigned int latency,
             unsigned int AHB_Prescaler_value, unsigned int APB2_Prescaler_value, unsigned int APB1_Prescaler_value)
{
  unsigned int cfgr, pllm, plln, pllq, pllp, use_hse, freq2;

  if (hclk_freq >= 84)
  {
    /* Select regulator voltage output Scale 1 mode */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;
  }
  else if (hclk_freq >= 60)
  {
    /* Select regulator voltage output Scale 2 mode */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS_1;
  }

  use_hse = oscill_freq != 0;
  if (!use_hse)
    oscill_freq = 16; // HSI clock
  else
  {
/* Конфигурацяи  SYSCLK, HCLK, PCLK2 и PCLK1 */
/* Включаем HSE */
    RCC->CR |= RCC_CR_HSEON;

/* Ждем пока HSE не выставит бит готовности */
    while (!(RCC->CR & RCC_CR_HSERDY));
  }

  FLASH->ACR |= FLASH_ACR_DCRST | FLASH_ACR_ICRST;

/* Включаем буфер предвыборки FLASH, data cache, instruction cache */
#ifndef DEBUG
  FLASH->ACR |= FLASH_ACR_PRFTEN | FLASH_ACR_DCEN | FLASH_ACR_ICEN;
#endif

/* Конфигурируем Flash 												*/
/* Это нужно потому, что Flash не может работать на высокой частоте 			*/
/* если это не сделать, то будет странный глюк. Проц может запуститься, но через пару 	*/
/* секунд повисает без "видимых причин". Вот такие вот неочевидные вилы. 		*/
  FLASH->ACR &= ~FLASH_ACR_LATENCY;
  FLASH->ACR |= latency;
 
/* HCLK = SYSCLK 															*/
/* AHB Prescaler = 1 														*/
  RCC->CFGR |= AHB_Prescaler_value;
 
/* APB2 Prescaler 														*/
  RCC->CFGR |= APB2_Prescaler_value;	   	
 
/* APB1 Prescaler 														*/
  RCC->CFGR |= APB1_Prescaler_value;		
 
/* Конфигурируем множитель PLL */
 
/* RCC_CFGR_PLLSRC -- выбираем HSE на вход 	*/
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
 
/* Все настроили? Включаем PLL */
  RCC->CR |= RCC_CR_PLLON;
 
/* Ожидаем, пока PLL выставит бит готовности */
  while(!(RCC->CR & RCC_CR_PLLRDY));
 
/* Работает? Можно переключать! Выбираем PLL как источник системной частоты */
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
 
/* Ожидаем, пока PLL выберется как источник системной частоты */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}
