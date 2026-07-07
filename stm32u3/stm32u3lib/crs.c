#include "board.h"
#include <crs.h>

void crs_lse_init(void)
{
  RCC->CR |= RCC_CR_HSI48ON;
  while (!(RCC->CR & RCC_CR_HSI48RDY))
    ;

  // 4. Enable CRS Peripheral Clock
  RCC->APB1ENR1 |= RCC_APB1ENR1_CRSEN;

  // 1. Configure the CRS Configuration Register (CRS_CFGR)
  // Set LSE as sync source (Source = 1)
  unsigned int temp = CRS_CFGR_SYNCSRC_0;

  // Set Rising Edge Polarity (Polarity = 0, default)
  temp |= 0 << CRS_CFGR_SYNCPOL_Pos;
  // Set Div 32 (Prescaler = 101)
  temp |= 5 << CRS_CFGR_SYNCDIV_Pos;

  // Set RELOAD value
  temp |= 0xB71A;

  // Set FELIM (Error Limit).
  temp |= 33 << CRS_CFGR_FELIM_Pos;

  CRS->CFGR = temp;

  // 2. Enable Auto-Trimming and Frequency Error Counter
  CRS->CR |= CRS_CR_AUTOTRIMEN | CRS_CR_CEN;

  //CRS->CR |= CRS_CR_SWSYNC;

  while (!(CRS->ISR & CRS_ISR_SYNCOKF))
    ;
}
