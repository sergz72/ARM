#include "board.h"
#include "sdram.h"

#ifdef SDRAM_TEST
int SDRAM_Test(void)
{
  unsigned int i, j;

  // 32 bits access
  for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned int))
    *(unsigned int *)(SDRAM_BANK_ADDR+i) = i;

  for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned int ))
  {
    if (*(unsigned int *)(SDRAM_BANK_ADDR+i) != i)
      return FALSE;
  }

  delayms(100); // delay 100 ms

  for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned int ))
  {
    if (*(unsigned int *)(SDRAM_BANK_ADDR+i) != i)
      return FALSE;
  }

  // 16 bits access
  for (j = 0; j <= 16; j += 4)
  {
    for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned short))
      *(unsigned short*)(SDRAM_BANK_ADDR+i) = (i >> j) & 0xFFFF;
  
    for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned short))
    {
      if (*(unsigned short*)(SDRAM_BANK_ADDR+i) != ((i >> j) & 0xFFFF))
        return FALSE;
    }
  }

  // 8 bits access
  for (j = 0; j <= 16; j += 4)
  {
    for ( i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned char))
      *(unsigned char*)(SDRAM_BANK_ADDR+i) = (i >> j) & 0xFF;
  
    for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned char))
    {
      if (*(unsigned char*)(SDRAM_BANK_ADDR+i) != ((i >> j) & 0xFF))
        return FALSE;
    }
  }

  return TRUE;
}
#endif
