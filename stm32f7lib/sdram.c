#include "board.h"
#include "sdram.h"

/**
  * @brief  Performs the SDRAM device initialization sequence.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  Timing: Pointer to SDRAM control timing structure 
  * @retval HAL status
  */
unsigned int SDRAM_Init(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_TimingTypeDef *Timing)
{   
  /* Check the SDRAM handle parameter */
  if(hsdram == NULL)
  {
    return 1;
  }
  
  if(hsdram->State == SDRAM_STATE_RESET)
  {  
    /* Initialize the low level hardware (MSP) */
    SDRAM_MspInit(hsdram);
  }
  
  /* Initialize the SDRAM controller state */
  hsdram->State = SDRAM_STATE_BUSY;
  
  /* Initialize SDRAM control Interface */
  FMC_SDRAM_Init(hsdram->Instance, &(hsdram->Init));
  
  /* Initialize SDRAM timing Interface */
  FMC_SDRAM_Timing_Init(hsdram->Instance, Timing, hsdram->Init.SDBank); 
  
  /* Update the SDRAM controller state */
  hsdram->State = SDRAM_STATE_READY;
  
  return 0;
}

/**
  * @brief  SDRAM MSP Init.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @retval None
  */
__weak void SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram)
{
}

/**
  * @brief  Sends Command to the SDRAM bank.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  Command: SDRAM command structure
  * @param  Timeout: Timeout duration
  * @retval HAL status
  */
unsigned int SDRAM_SendCommand(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command, unsigned int Timeout)
{
  /* Check the SDRAM controller state */
  if(hsdram->State == SDRAM_STATE_BUSY)
  {
    return 1;
  }

  /* Update the SDRAM state */
  hsdram->State = SDRAM_STATE_BUSY;

  /* Send SDRAM command */
  FMC_SDRAM_SendCommand(hsdram->Instance, Command, Timeout);

  /* Update the SDRAM controller state state */
  if(Command->CommandMode == FMC_SDRAM_CMD_PALL)
  {
    hsdram->State = SDRAM_STATE_PRECHARGED;
  }
  else
  {
    hsdram->State = SDRAM_STATE_READY;
  }

  return 0;
}

/**
  * @brief  Programs the SDRAM Memory Refresh rate.
  * @param  hsdram: pointer to a SDRAM_HandleTypeDef structure that contains
  *                the configuration information for SDRAM module.
  * @param  RefreshRate: The SDRAM refresh rate value
  * @retval HAL status
  */
unsigned int SDRAM_ProgramRefreshRate(SDRAM_HandleTypeDef *hsdram, unsigned int RefreshRate)
{
  /* Check the SDRAM controller state */
  if(hsdram->State == SDRAM_STATE_BUSY)
  {
    return 1;
  }

  /* Update the SDRAM state */
  hsdram->State = SDRAM_STATE_BUSY;

  /* Program the refresh rate */
  FMC_SDRAM_ProgramRefreshRate(hsdram->Instance ,RefreshRate);

  /* Update the SDRAM state */
  hsdram->State = SDRAM_STATE_READY;

  return 0;
}

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
