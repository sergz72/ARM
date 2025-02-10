#ifndef _FMC_H
#define _FMC_H

#define FMC_BCR_NBLSET(n) (n << 22)
#define FMC_BCR_MWID8 0
#define FMC_BCR_MWID16 (1 << 4)
#define FMC_BCR_MTYP_SRAM 0
#define FMC_BCR_MTYP_PSRAM 4
#define FMC_BCR_MTYP_NOR_NAND 8

#define FMC_BTR_DATAHLD(n) ((unsigned int)n << 30)
#define FMC_BTR_ACCMOD(n) (n << 28)
#define FMC_BTR_DATALAT(n) (n << 24)
#define FMC_BTR_CLKDIV(n) (n << 20)
#define FMC_BTR_BUSTURN(n) (n << 16)
#define FMC_BTR_DATAST(n) (n << 8)
#define FMC_BTR_ADDHLD(n) (n << 4)
#define FMC_BTR_ADDSET(n) n

#define FMC_BWTR_DATAHLD(n) (n << 30)
#define FMC_BWTR_ACCMOD(n) (n << 28)
#define FMC_BWTR_BUSTURN(n) (n << 16)
#define FMC_BWTR_DATAST(n) (n << 8)
#define FMC_BWTR_ADDHLD(n) (n << 4)
#define FMC_BWTR_ADDSET(n) n

typedef struct
{
  unsigned int BCR;
  unsigned int BTR;
  unsigned int BWTR;
} SRAMConfig;

void SRAMInit(unsigned int bankId, const SRAMConfig *config);
void FMCEnable(void);

#endif
