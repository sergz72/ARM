#include <board.h>
#include <pll.h>

void InitRCC(unsigned int multipler, unsigned int hpre, unsigned int ppre1, unsigned int ppre2) 
{
  int cfgr;

  if (multipler < 2 || multipler > 16)
// bad multipler value
    while (1);

/* Конфигурация  SYSCLK, HCLK, PCLK2 и PCLK1 */
#ifdef PLL_SOURCE_HSI
/* Включаем HSI */
  RCC->CR |= RCC_CR_HSION;
/* Ждем пока HSI не выставит бит готовности */
  while (!(RCC->CR & RCC_CR_HSIRDY));
#else
#ifdef HSEBYPASS
  RCC->CR |= RCC_CR_HSEBYP;
#endif
/* Включаем HSE */
  RCC->CR |= RCC_CR_HSEON;
/* Ждем пока HSE не выставит бит готовности */
  while (!(RCC->CR & RCC_CR_HSERDY));
#endif

/* Включаем буфер предвыборки FLASH */
//  FLASH->ACR |= FLASH_ACR_PRFTBE;

#ifndef FLASH_LATENCY_0
/* Конфигурируем Flash на 2 цикла ожидания 												*/
/* Это нужно потому, что Flash не может работать на высокой частоте 			*/
/* если это не сделать, то будет странный глюк. Проц может запуститься, но через пару 	*/
/* секунд повисает без "видимых причин". Вот такие вот неочевидные вилы. 		*/
  FLASH->ACR &= ~FLASH_ACR_LATENCY;
  FLASH->ACR |= FLASH_ACR_LATENCY_2;
#endif

/* AHB Prescaler 														*/
  RCC->CFGR |= hpre;		

/* APB2 Prescaler 														*/
  RCC->CFGR |= ppre2;	   	
 
/* APB1 Prescaler 														*/
  RCC->CFGR |= ppre1;		

/* Конфигурируем множитель PLL */
 
/* Сбрасываем в нули прежнее значение*/
  RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL);

/* А теперь накатываем новое 												*/
#ifdef PLL_SOURCE_HSE
/* RCC_CFGR_PLLSRC -- выбираем HSE на вход 	*/
  cfgr = RCC_CFGR_PLLSRC | ((multipler - 2)  << 18);
#else
  cfgr = (multipler - 2)  << 18;
#endif
  RCC->CFGR |= cfgr;
 
/* Все настроили? Включаем PLL */
  RCC->CR |= RCC_CR_PLLON;
 
/* Ожидаем, пока PLL выставит бит готовности */
  while(!(RCC->CR & RCC_CR_PLLRDY));
 
/* Работает? Можно переключать! Выбираем PLL как источник системной частоты */
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
 
/* Ожидаем, пока PLL выберется как источник системной частоты */
  while ((RCC->CFGR & RCC_CFGR_SWS) != 8); 
}
