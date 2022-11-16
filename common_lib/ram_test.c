#define _LOG_TO_SCREEN_
#include <board.h>
#include <ram_test.h>

#define RAM_BASE_ADDR *(volatile unsigned int*)SDRAM_BASE_ADDRESS

int RAM_Test(void)
{
  unsigned int i, j;

  PutStr_Log("RAM test...\n");
  PutStr_Log("32 bit access...\n");

  // 32 bits access
  for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned int))
    *(unsigned int *)((unsigned int)&RAM_BASE_ADDR+i) = i;

  for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned int ))
  {
    if (*(unsigned int *)((unsigned int )&RAM_BASE_ADDR+i) != i)
    {
      PRINT_Log("i = %X value = %X\n", i, *(unsigned int *)((unsigned int )&RAM_BASE_ADDR+i));
      return FALSE;
    }
  }

  PutStr_Log("Delay 1 sec...\n");

  delay(1000000); // delay 1 sec

  PutStr_Log("Data persistency check...\n");

  for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned int ))
  {
    if (*(unsigned int *)((unsigned int )&RAM_BASE_ADDR+i) != i)
    {
      PRINT_Log("i = %X value = %X\n", i, *(unsigned int *)((unsigned int )&RAM_BASE_ADDR+i));
      return FALSE;
    }
  }

  PutStr_Log("16 bit access...\n");
  // 16 bits access
  for (j = 0; j <= 16; j += 4)
  {
    PRINT_Log("Shift = %d...\n", j);
    for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned short))
      *(unsigned short*)((unsigned int)&RAM_BASE_ADDR+i) = (i >> j) & 0xFFFF;
  
    for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned short))
    {
      if (*(unsigned short*)((unsigned int)&RAM_BASE_ADDR+i) != ((i >> j) & 0xFFFF))
      {
        PRINT_Log("i = %X value = %X\n", i, *(unsigned int *)((unsigned int )&RAM_BASE_ADDR+i));
        return FALSE;
      }
    }
  }

  PutStr_Log("8 bit access...\n");

  // 8 bits access
  for (j = 0; j <= 16; j += 4)
  {
    PRINT_Log("Shift = %d...\n", j);
    for ( i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned char))
      *(unsigned char*)((unsigned int)&RAM_BASE_ADDR+i) = (i >> j) & 0xFF;
  
    for (i = 0; i < SDRAM_SIZE; i+=sizeof(unsigned char))
    {
      if (*(unsigned char*)((unsigned int)&RAM_BASE_ADDR+i) != ((i >> j) & 0xFF))
      {
        PRINT_Log("i = %X value = %X\n", i, *(unsigned int *)((unsigned int )&RAM_BASE_ADDR+i));
        return FALSE;
      }
    }
  }

  PutStr_Log("RAM test passed.\n");

  return TRUE;
}
