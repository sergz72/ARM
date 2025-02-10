#include "board.h"
#include "fmc.h"

void SRAMInit(unsigned int bankId, const SRAMConfig *config)
{
  bankId <<= 1;
  FMC_Bank1_R->BTCR[bankId+1] = config->BTR;
  FMC_Bank1_R->BTCR[bankId] = config->BCR;
  if (config->BCR & FMC_BCRx_EXTMOD)
    FMC_Bank1E_R->BWTR[bankId] = config->BWTR;
}

void FMCEnable(void)
{
  FMC_Bank1_R->BTCR[0] |= 0x80000000;
}
