#include "board.h"
#include <pwr.h>

void enter_standby(void)
{
  /* Select Standby sleep mode */
  PM_REGS->PM_SLEEPCFG = PM_SLEEPCFG_SLEEPMODE_STANDBY;

  /* Wait until synchronized */
  while ((PM_REGS->PM_SLEEPCFG & PM_SLEEPCFG_SLEEPMODE_Msk) != PM_SLEEPCFG_SLEEPMODE_STANDBY)
    ;

  /* Ensure all memory accesses complete */
  __DSB();

  /* Enter sleep */
  __WFI();

  /* Select Standby sleep mode */
  PM_REGS->PM_SLEEPCFG = PM_SLEEPCFG_SLEEPMODE_IDLE;

  /* Wait until synchronized */
  while ((PM_REGS->PM_SLEEPCFG & PM_SLEEPCFG_SLEEPMODE_Msk) != PM_SLEEPCFG_SLEEPMODE_IDLE)
    ;
}
