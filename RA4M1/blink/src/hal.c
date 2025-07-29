#include "board.h"

static void ClockInit(void)
{
  R_SYSTEM->PRCR = 0xA501; // Enable writing to the clock registers
  //R_SYSTEM->HOCOCR2 = 0; // 24 MHz
  //R_SYSTEM->HOCOCR = 0; // enabled
  // default system clock is MOCO (8MHz)
  //R_SYSTEM->SCKSCR = 0;
  // set all clock dividers to 1
  R_SYSTEM->SCKDIVCR = 0;
  //R_SYSTEM->SCKDIVCR_b.ICK = 0;
  //R_SYSTEM->SCKDIVCR_b.FCK = 0;
  //R_SYSTEM->SCKDIVCR_b.PCKA = 0;
  //R_SYSTEM->SCKDIVCR_b.PCKB = 0;
  //R_SYSTEM->SCKDIVCR_b.PCKC = 0;
  //R_SYSTEM->SCKDIVCR_b.PCKD = 0;*/
  R_SYSTEM->PRCR = 0xA500; // Disable writing to the clock registers
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
}

static void GPIOInit(void)
{
  //R_PMISC->PWPR = 0x00;        // Clear B0WI
  //R_PMISC->PWPR = 0x40;        // Set PFSWE

  /* Configure P012 as a GPIO output */
  //R_PFS->PORT[LED_PORT].PIN[LED_PIN].PmnPFS = 0x00;  // GPIO function

  /* Set P012 as output */
  R_PORT0->PDR |= (1 << LED_PIN);   // Direction: Output
}

void SystemInit(void)
{
  ClockInit();
  GPIOInit();
}

void _init(void)
{

}
