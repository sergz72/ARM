#include "board.h"
#include <system.h>

typedef void (* exc_ptr_t)(void);

extern void  * __VECTOR_TABLE[];

const exc_ptr_t g_vector_table[0] __attribute__((section(".application_vectors"))) =
{
};

static void ClockInit(void)
{
  R_SYSTEM->PRCR = 0xA501; // Enable writing to the clock registers
  R_SYSTEM->HOCOCR2 = 0x20; // 48 MHz
  R_SYSTEM->HOCOCR = 0; // enabled
  R_SYSTEM->MEMWAIT = 1; // 1 wait cycle
  // switch to HOCO (48 MHz)
  R_SYSTEM->SCKSCR = 0;
  R_SYSTEM->SCKDIVCR_b.ICK = 0; // 48 MHz clock
  R_SYSTEM->SCKDIVCR_b.FCK = 1; // 24 MHz clock
  R_SYSTEM->SCKDIVCR_b.PCKA = 0; // 48 MHz clock
  R_SYSTEM->SCKDIVCR_b.PCKB = 1; // 24 MHz clock
  R_SYSTEM->SCKDIVCR_b.PCKC = 0; // 48 MHz clock
  R_SYSTEM->SCKDIVCR_b.PCKD = 0; // 48 MHz clock
  // invalidate flash cache
  R_FCACHE->FCACHEIV = 1;
  while (R_FCACHE->FCACHEIV & 1)
    ;
  // enable flash cache
  R_FCACHE->FCACHEE = 1;
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
  SCB->VTOR = (uint32_t) &__VECTOR_TABLE;

  ClockInit();
  SystemRuntimeInit(0);
  GPIOInit();
}

void _init(void)
{

}
