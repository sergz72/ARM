#include "board.h"
#include <trng.h>

static volatile unsigned int *datap;
static volatile unsigned int length;

void TRNG_Handler(void)
{
  if (!length)
  {
    TRNG_REGS->TRNG_INTFLAG = TRNG_INTFLAG_DATARDY(1);
    return;
  }
  *datap++ = TRNG_REGS->TRNG_DATA;
  length--;
  if (!length)
    TRNG_REGS->TRNG_INTENCLR = TRNG_INTENCLR_DATARDY(1);
}

void trng_init(void)
{
  TRNG_REGS->TRNG_INTFLAG = TRNG_INTFLAG_DATARDY(1);
  NVIC_SetPriority(TRNG_IRQn, TRNG_INTERRUPT_PRIORITY);
  NVIC_EnableIRQ(TRNG_IRQn);
}

void trng_generate(unsigned int *data, unsigned int l)
{
  datap = data;
  length = l;
  TRNG_REGS->TRNG_INTENSET = TRNG_INTENSET_DATARDY(1);
  TRNG_REGS->TRNG_CTRLA = TRNG_CTRLA_ENABLE(1);
  while (length)
    __WFI();
  TRNG_REGS->TRNG_CTRLA = 0;
}
