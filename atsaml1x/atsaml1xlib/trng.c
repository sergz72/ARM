#include "board.h"
#include <trng.h>

void trng_generate(unsigned int *data, unsigned int length)
{
  TRNG_REGS->TRNG_INTFLAG = TRNG_INTFLAG_DATARDY(1);
  TRNG_REGS->TRNG_CTRLA = TRNG_CTRLA_ENABLE(1);
  while (length--)
  {
    while (!(TRNG_REGS->TRNG_INTFLAG & TRNG_INTFLAG_DATARDY(1)))
      ;
    *data++ = TRNG_REGS->TRNG_DATA;
  };
  TRNG_REGS->TRNG_CTRLA = 0;
}
