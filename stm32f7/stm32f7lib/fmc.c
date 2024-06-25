#include "board.h"
#include "fmc.h"

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize the FMC_NORSRAM device according to the specified
  *         control parameters in the FMC_NORSRAM_InitTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Init: Pointer to NORSRAM Initialization structure   
  * @retval HAL status
  */
void FMC_NORSRAM_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_InitTypeDef* Init)
{ 
  unsigned int tmpr;
    
  /* Get the BTCR register value */
  tmpr = Device->BTCR[Init->NSBank];
  
  /* Clear MBKEN, MUXEN, MTYP, MWID, FACCEN, BURSTEN, WAITPOL, WAITCFG, WREN,
           WAITEN, EXTMOD, ASYNCWAIT, CBURSTRW and CCLKEN bits */
  tmpr &= ((unsigned int)~(FMC_BCR1_MBKEN     | FMC_BCR1_MUXEN    | FMC_BCR1_MTYP     | \
                       FMC_BCR1_MWID      | FMC_BCR1_FACCEN   | FMC_BCR1_BURSTEN  | \
                       FMC_BCR1_WAITPOL   | FMC_BCR1_CPSIZE    | FMC_BCR1_WAITCFG  | \
                       FMC_BCR1_WREN      | FMC_BCR1_WAITEN   | FMC_BCR1_EXTMOD   | \
                       FMC_BCR1_ASYNCWAIT | FMC_BCR1_CBURSTRW | FMC_BCR1_CCLKEN | FMC_BCR1_WFDIS));
  
  /* Set NORSRAM device control parameters */
  tmpr |= (unsigned int)(Init->DataAddressMux       |\
                    Init->MemoryType           |\
                    Init->MemoryDataWidth      |\
                    Init->BurstAccessMode      |\
                    Init->WaitSignalPolarity   |\
                    Init->WaitSignalActive     |\
                    Init->WriteOperation       |\
                    Init->WaitSignal           |\
                    Init->ExtendedMode         |\
                    Init->AsynchronousWait     |\
                    Init->WriteBurst           |\
                    Init->ContinuousClock      |\
                    Init->PageSize             |\
                    Init->WriteFifo);
                    
  if(Init->MemoryType == FMC_MEMORY_TYPE_NOR)
  {
    tmpr |= (unsigned int)FMC_NORSRAM_FLASH_ACCESS_ENABLE;
  }
  
  Device->BTCR[Init->NSBank] = tmpr;

  /* Configure synchronous mode when Continuous clock is enabled for bank2..4 */
  if((Init->ContinuousClock == FMC_CONTINUOUS_CLOCK_SYNC_ASYNC) && (Init->NSBank != FMC_NORSRAM_BANK1))
  { 
    Device->BTCR[FMC_NORSRAM_BANK1] |= (unsigned int)(Init->ContinuousClock);
  }
  if(Init->NSBank != FMC_NORSRAM_BANK1)
  {
    Device->BTCR[FMC_NORSRAM_BANK1] |= (unsigned int)(Init->WriteFifo);              
  }
}


/**
  * @brief  DeInitialize the FMC_NORSRAM peripheral 
  * @param  Device: Pointer to NORSRAM device instance
  * @param  ExDevice: Pointer to NORSRAM extended mode device instance  
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
void FMC_NORSRAM_DeInit(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_EXTENDED_TypeDef *ExDevice, unsigned int Bank)
{
  /* Disable the FMC_NORSRAM device */
  __FMC_NORSRAM_DISABLE(Device, Bank);
  
  /* De-initialize the FMC_NORSRAM device */
  /* FMC_NORSRAM_BANK1 */
  if(Bank == FMC_NORSRAM_BANK1)
  {
    Device->BTCR[Bank] = 0x000030DB;    
  }
  /* FMC_NORSRAM_BANK2, FMC_NORSRAM_BANK3 or FMC_NORSRAM_BANK4 */
  else
  {   
    Device->BTCR[Bank] = 0x000030D2; 
  }
  
  Device->BTCR[Bank + 1] = 0x0FFFFFFF;
  ExDevice->BWTR[Bank]   = 0x0FFFFFFF;
}


/**
  * @brief  Initialize the FMC_NORSRAM Timing according to the specified
  *         parameters in the FMC_NORSRAM_TimingTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Timing: Pointer to NORSRAM Timing structure
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
void FMC_NORSRAM_Timing_Init(FMC_NORSRAM_TypeDef *Device, FMC_NORSRAM_TimingTypeDef *Timing, unsigned int Bank)
{
  unsigned int tmpr;
  
  /* Get the BTCR register value */
  tmpr = Device->BTCR[Bank + 1];

  /* Clear ADDSET, ADDHLD, DATAST, BUSTURN, CLKDIV, DATLAT and ACCMOD bits */
  tmpr &= ((unsigned int)~(FMC_BTR1_ADDSET  | FMC_BTR1_ADDHLD | FMC_BTR1_DATAST | \
                       FMC_BTR1_BUSTURN | FMC_BTR1_CLKDIV | FMC_BTR1_DATLAT | \
                       FMC_BTR1_ACCMOD));
  
  /* Set FMC_NORSRAM device timing parameters */  
  tmpr |= (unsigned int)(Timing->AddressSetupTime                  |\
                   ((Timing->AddressHoldTime) << 4)          |\
                   ((Timing->DataSetupTime) << 8)            |\
                   ((Timing->BusTurnAroundDuration) << 16)   |\
                   (((Timing->CLKDivision)-1) << 20)         |\
                   (((Timing->DataLatency)-2) << 24)         |\
                    (Timing->AccessMode)
                    );
  
  Device->BTCR[Bank + 1] = tmpr;
  
  /* Configure Clock division value (in NORSRAM bank 1) when continuous clock is enabled */
  if(Device->BTCR[FMC_NORSRAM_BANK1] & FMC_BCR1_CCLKEN)
  {
    tmpr = (unsigned int)(Device->BTCR[FMC_NORSRAM_BANK1 + 1] & ~(((unsigned int)0x0F) << 20)); 
    tmpr |= (unsigned int)(((Timing->CLKDivision)-1) << 20);
    Device->BTCR[FMC_NORSRAM_BANK1 + 1] = tmpr;
  }  
}

/**
  * @brief  Initialize the FMC_NORSRAM Extended mode Timing according to the specified
  *         parameters in the FMC_NORSRAM_TimingTypeDef
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Timing: Pointer to NORSRAM Timing structure
  * @param  Bank: NORSRAM bank number  
  * @retval HAL status
  */
void FMC_NORSRAM_Extended_Timing_Init(FMC_NORSRAM_EXTENDED_TypeDef *Device, FMC_NORSRAM_TimingTypeDef *Timing, unsigned int Bank, unsigned int ExtendedMode)
{  
  unsigned int tmpr;
 
  /* Set NORSRAM device timing register for write configuration, if extended mode is used */
  if(ExtendedMode == FMC_EXTENDED_MODE_ENABLE)
  {
    /* Get the BWTR register value */
    tmpr = Device->BWTR[Bank];

    /* Clear ADDSET, ADDHLD, DATAST, BUSTURN, CLKDIV, DATLAT and ACCMOD bits */
    tmpr &= ((unsigned int)~(FMC_BWTR1_ADDSET  | FMC_BWTR1_ADDHLD | FMC_BWTR1_DATAST | \
                         FMC_BWTR1_BUSTURN | FMC_BWTR1_ACCMOD));
    
    tmpr |= (unsigned int)(Timing->AddressSetupTime                 |\
                      ((Timing->AddressHoldTime) << 4)          |\
                      ((Timing->DataSetupTime) << 8)            |\
                      ((Timing->BusTurnAroundDuration) << 16)   |\
                      (Timing->AccessMode));

    Device->BWTR[Bank] = tmpr;
  }
  else
  {
    Device->BWTR[Bank] = 0x0FFFFFFF;
  }   
}

/**
  * @brief  Enables dynamically FMC_NORSRAM write operation.
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Bank: NORSRAM bank number   
  * @retval HAL status
  */
void FMC_NORSRAM_WriteOperation_Enable(FMC_NORSRAM_TypeDef *Device, unsigned int Bank)
{
  /* Enable write operation */
  Device->BTCR[Bank] |= FMC_WRITE_OPERATION_ENABLE; 
}

/**
  * @brief  Disables dynamically FMC_NORSRAM write operation.
  * @param  Device: Pointer to NORSRAM device instance
  * @param  Bank: NORSRAM bank number   
  * @retval HAL status
  */
void FMC_NORSRAM_WriteOperation_Disable(FMC_NORSRAM_TypeDef *Device, unsigned int Bank)
{ 
  /* Disable write operation */
  Device->BTCR[Bank] &= ~FMC_WRITE_OPERATION_ENABLE; 
}

/**
  * @brief  Initializes the FMC_NAND device according to the specified
  *         control parameters in the FMC_NAND_HandleTypeDef
  * @param  Device: Pointer to NAND device instance
  * @param  Init: Pointer to NAND Initialization structure
  * @retval HAL status
  */
void FMC_NAND_Init(FMC_NAND_TypeDef *Device, FMC_NAND_InitTypeDef *Init)
{
  unsigned int tmpr;
    
  /* Get the NAND bank 3 register value */
  tmpr = Device->PCR;

  /* Clear PWAITEN, PBKEN, PTYP, PWID, ECCEN, TCLR, TAR and ECCPS bits */
  tmpr &= ((unsigned int)~(FMC_PCR_PWAITEN  | FMC_PCR_PBKEN | FMC_PCR_PTYP | \
                       FMC_PCR_PWID | FMC_PCR_ECCEN | FMC_PCR_TCLR | \
                       FMC_PCR_TAR | FMC_PCR_ECCPS));  
  /* Set NAND device control parameters */
  tmpr |= (unsigned int)(Init->Waitfeature                |\
                      FMC_PCR_MEMORY_TYPE_NAND         |\
                      Init->MemoryDataWidth            |\
                      Init->EccComputation             |\
                      Init->ECCPageSize                |\
                      ((Init->TCLRSetupTime) << 9)     |\
                      ((Init->TARSetupTime) << 13));   
  
    /* NAND bank 3 registers configuration */
    Device->PCR  = tmpr;
}

/**
  * @brief  Initializes the FMC_NAND Common space Timing according to the specified
  *         parameters in the FMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to NAND device instance
  * @param  Timing: Pointer to NAND timing structure
  * @param  Bank: NAND bank number   
  * @retval HAL status
  */
void FMC_NAND_CommonSpace_Timing_Init(FMC_NAND_TypeDef *Device, FMC_NAND_PCC_TimingTypeDef *Timing, unsigned int Bank)
{
  unsigned int tmpr;
  
  /* Get the NAND bank 3 register value */
  tmpr = Device->PMEM;

  /* Clear MEMSETx, MEMWAITx, MEMHOLDx and MEMHIZx bits */
  tmpr &= ((unsigned int)~(FMC_PMEM_MEMSET3  | FMC_PMEM_MEMWAIT3 | FMC_PMEM_MEMHOLD3 | \
                       FMC_PMEM_MEMHIZ3)); 
  /* Set FMC_NAND device timing parameters */
  tmpr |= (unsigned int)(Timing->SetupTime                  |\
                       ((Timing->WaitSetupTime) << 8)     |\
                       ((Timing->HoldSetupTime) << 16)    |\
                       ((Timing->HiZSetupTime) << 24)
                       );
                            
  /* NAND bank 3 registers configuration */
  Device->PMEM = tmpr;
}

/**
  * @brief  Initializes the FMC_NAND Attribute space Timing according to the specified
  *         parameters in the FMC_NAND_PCC_TimingTypeDef
  * @param  Device: Pointer to NAND device instance
  * @param  Timing: Pointer to NAND timing structure
  * @param  Bank: NAND bank number 
  * @retval HAL status
  */
void FMC_NAND_AttributeSpace_Timing_Init(FMC_NAND_TypeDef *Device, FMC_NAND_PCC_TimingTypeDef *Timing, unsigned int Bank)
{
  unsigned int tmpr;
  
  /* Get the NAND bank 3 register value */
  tmpr = Device->PATT;

  /* Clear ATTSETx, ATTWAITx, ATTHOLDx and ATTHIZx bits */
  tmpr &= ((unsigned int)~(FMC_PATT_ATTSET3  | FMC_PATT_ATTWAIT3 | FMC_PATT_ATTHOLD3 | \
                       FMC_PATT_ATTHIZ3));
  /* Set FMC_NAND device timing parameters */
  tmpr |= (unsigned int)(Timing->SetupTime                  |\
                   ((Timing->WaitSetupTime) << 8)     |\
                   ((Timing->HoldSetupTime) << 16)    |\
                   ((Timing->HiZSetupTime) << 24));
                       
    /* NAND bank 3 registers configuration */
    Device->PATT = tmpr;
}

/**
  * @brief  DeInitializes the FMC_NAND device 
  * @param  Device: Pointer to NAND device instance
  * @param  Bank: NAND bank number
  * @retval HAL status
  */
void FMC_NAND_DeInit(FMC_NAND_TypeDef *Device, unsigned int Bank)
{
  /* Disable the NAND Bank */
  __FMC_NAND_DISABLE(Device);
 
  /* Set the FMC_NAND_BANK3 registers to their reset values */
  Device->PCR  = 0x00000018U;
  Device->SR   = 0x00000040U;
  Device->PMEM = 0xFCFCFCFCU;
  Device->PATT = 0xFCFCFCFCU;
}

/**
  * @brief  Enables dynamically FMC_NAND ECC feature.
  * @param  Device: Pointer to NAND device instance
  * @param  Bank: NAND bank number
  * @retval HAL status
  */    
void FMC_NAND_ECC_Enable(FMC_NAND_TypeDef *Device, unsigned int Bank)
{
  /* Enable ECC feature */
    Device->PCR |= FMC_PCR_ECCEN;
}


/**
  * @brief  Disables dynamically FMC_NAND ECC feature.
  * @param  Device: Pointer to NAND device instance
  * @param  Bank: NAND bank number
  * @retval HAL status
  */  
void FMC_NAND_ECC_Disable(FMC_NAND_TypeDef *Device, unsigned int Bank)
{  
  /* Disable ECC feature */
  Device->PCR &= ~FMC_PCR_ECCEN;
}

/**
  * @brief  Disables dynamically FMC_NAND ECC feature.
  * @param  Device: Pointer to NAND device instance
  * @param  ECCval: Pointer to ECC value
  * @param  Bank: NAND bank number
  * @param  Timeout: Timeout wait value  
  * @retval HAL status
  */
unsigned int FMC_NAND_GetECC(FMC_NAND_TypeDef *Device, unsigned int *ECCval, unsigned int Bank, unsigned int Timeout)
{
  unsigned int tickstart;

  /* Get tick */ 
  tickstart = 0;

  /* Wait until FIFO is empty */
  while(__FMC_NAND_GET_FLAG(Device, Bank, FMC_FLAG_FEMPT) == RESET)
  {
    /* Check for the Timeout */
    if(Timeout != _MAX_DELAY)
    {
      if((Timeout == 0)||(tickstart++ > Timeout))
      {
        return 1;
      }
    }  
  }
 
  /* Get the ECCR register value */
  *ECCval = (unsigned int)Device->ECCR;

  return 0;
}

/**
  * @brief  Initializes the FMC_SDRAM device according to the specified
  *         control parameters in the FMC_SDRAM_InitTypeDef
  * @param  Device: Pointer to SDRAM device instance
  * @param  Init: Pointer to SDRAM Initialization structure   
  * @retval HAL status
  */
void FMC_SDRAM_Init(FMC_SDRAM_TypeDef *Device, FMC_SDRAM_InitTypeDef *Init)
{
  unsigned int tmpr1;
  unsigned int tmpr2;
    
  /* Set SDRAM bank configuration parameters */
  if (Init->SDBank != FMC_SDRAM_BANK2) 
  {
    tmpr1 = Device->SDCR[FMC_SDRAM_BANK1];
    
    /* Clear NC, NR, MWID, NB, CAS, WP, SDCLK, RBURST, and RPIPE bits */
    tmpr1 &= ((unsigned int)~(FMC_SDCR1_NC  | FMC_SDCR1_NR | FMC_SDCR1_MWID | \
                          FMC_SDCR1_NB  | FMC_SDCR1_CAS | FMC_SDCR1_WP   | \
                          FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE));

    tmpr1 |= (unsigned int)(Init->ColumnBitsNumber   |\
                        Init->RowBitsNumber      |\
                        Init->MemoryDataWidth    |\
                        Init->InternalBankNumber |\
                        Init->CASLatency         |\
                        Init->WriteProtection    |\
                        Init->SDClockPeriod      |\
                        Init->ReadBurst          |\
                        Init->ReadPipeDelay
                        );                                      
    Device->SDCR[FMC_SDRAM_BANK1] = tmpr1;
  }
  else /* FMC_Bank2_SDRAM */                      
  {
    tmpr1 = Device->SDCR[FMC_SDRAM_BANK1];
    
    /* Clear SDCLK, RBURST, and RPIPE bits */
    tmpr1 &= ((unsigned int)~(FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE));
    
    tmpr1 |= (unsigned int)(Init->SDClockPeriod      |\
                        Init->ReadBurst          |\
                        Init->ReadPipeDelay);
    
    tmpr2 = Device->SDCR[FMC_SDRAM_BANK2];
    
    /* Clear NC, NR, MWID, NB, CAS, WP, SDCLK, RBURST, and RPIPE bits */
    tmpr2 &= ((unsigned int)~(FMC_SDCR1_NC  | FMC_SDCR1_NR | FMC_SDCR1_MWID | \
                          FMC_SDCR1_NB  | FMC_SDCR1_CAS | FMC_SDCR1_WP   | \
                          FMC_SDCR1_SDCLK | FMC_SDCR1_RBURST | FMC_SDCR1_RPIPE));

    tmpr2 |= (unsigned int)(Init->ColumnBitsNumber   |\
                       Init->RowBitsNumber       |\
                       Init->MemoryDataWidth     |\
                       Init->InternalBankNumber  |\
                       Init->CASLatency          |\
                       Init->WriteProtection);

    Device->SDCR[FMC_SDRAM_BANK1] = tmpr1;
    Device->SDCR[FMC_SDRAM_BANK2] = tmpr2;
  }
}

/**
  * @brief  Initializes the FMC_SDRAM device timing according to the specified
  *         parameters in the FMC_SDRAM_TimingTypeDef
  * @param  Device: Pointer to SDRAM device instance
  * @param  Timing: Pointer to SDRAM Timing structure
  * @param  Bank: SDRAM bank number   
  * @retval HAL status
  */
void FMC_SDRAM_Timing_Init(FMC_SDRAM_TypeDef *Device, FMC_SDRAM_TimingTypeDef *Timing, unsigned int Bank)
{
  unsigned int tmpr1;
  unsigned int tmpr2;
    
  /* Set SDRAM device timing parameters */ 
  if (Bank != FMC_SDRAM_BANK2) 
  {
    tmpr1 = Device->SDTR[FMC_SDRAM_BANK1];
    
    /* Clear TMRD, TXSR, TRAS, TRC, TWR, TRP and TRCD bits */
    tmpr1 &= ((unsigned int)~(FMC_SDTR1_TMRD  | FMC_SDTR1_TXSR | FMC_SDTR1_TRAS | \
                          FMC_SDTR1_TRC  | FMC_SDTR1_TWR | FMC_SDTR1_TRP | \
                          FMC_SDTR1_TRCD));
    
    tmpr1 |= (unsigned int)(((Timing->LoadToActiveDelay)-1)           |\
                       (((Timing->ExitSelfRefreshDelay)-1) << 4) |\
                       (((Timing->SelfRefreshTime)-1) << 8)      |\
                       (((Timing->RowCycleDelay)-1) << 12)       |\
                       (((Timing->WriteRecoveryTime)-1) <<16)    |\
                       (((Timing->RPDelay)-1) << 20)             |\
                       (((Timing->RCDDelay)-1) << 24));
    Device->SDTR[FMC_SDRAM_BANK1] = tmpr1;
  }
  else /* FMC_Bank2_SDRAM */
  {
    tmpr1 = Device->SDTR[FMC_SDRAM_BANK1];
    
    /* Clear TRC and TRP bits */
    tmpr1 &= ((unsigned int)~(FMC_SDTR1_TRC | FMC_SDTR1_TRP));
    
    tmpr1 |= (unsigned int)((((Timing->RowCycleDelay)-1) << 12)       |\
                        (((Timing->RPDelay)-1) << 20)); 
    
    tmpr2 = Device->SDTR[FMC_SDRAM_BANK2];
    
    /* Clear TMRD, TXSR, TRAS, TRC, TWR, TRP and TRCD bits */
    tmpr2 &= ((unsigned int)~(FMC_SDTR1_TMRD  | FMC_SDTR1_TXSR | FMC_SDTR1_TRAS | \
                          FMC_SDTR1_TRC  | FMC_SDTR1_TWR | FMC_SDTR1_TRP | \
                          FMC_SDTR1_TRCD));
    
    tmpr2 |= (unsigned int)(((Timing->LoadToActiveDelay)-1)           |\
                       (((Timing->ExitSelfRefreshDelay)-1) << 4)  |\
                       (((Timing->SelfRefreshTime)-1) << 8)       |\
                       (((Timing->WriteRecoveryTime)-1) <<16)     |\
                       (((Timing->RCDDelay)-1) << 24));   

    Device->SDTR[FMC_SDRAM_BANK1] = tmpr1;
    Device->SDTR[FMC_SDRAM_BANK2] = tmpr2;
  }
}

/**
  * @brief  DeInitializes the FMC_SDRAM peripheral 
  * @param  Device: Pointer to SDRAM device instance
  * @retval HAL status
  */
void FMC_SDRAM_DeInit(FMC_SDRAM_TypeDef *Device, unsigned int Bank)
{
  /* De-initialize the SDRAM device */
  Device->SDCR[Bank] = 0x000002D0;
  Device->SDTR[Bank] = 0x0FFFFFFF;    
  Device->SDCMR      = 0x00000000;
  Device->SDRTR      = 0x00000000;
  Device->SDSR       = 0x00000000;
}

/**
  * @brief  Enables dynamically FMC_SDRAM write protection.
  * @param  Device: Pointer to SDRAM device instance
  * @param  Bank: SDRAM bank number 
  * @retval HAL status
  */
void FMC_SDRAM_WriteProtection_Enable(FMC_SDRAM_TypeDef *Device, unsigned int Bank)
{ 
  /* Enable write protection */
  Device->SDCR[Bank] |= FMC_SDRAM_WRITE_PROTECTION_ENABLE;
}

/**
  * @brief  Disables dynamically FMC_SDRAM write protection.
  * @param  hsdram: FMC_SDRAM handle
  * @retval HAL status
  */
void FMC_SDRAM_WriteProtection_Disable(FMC_SDRAM_TypeDef *Device, unsigned int Bank)
{
  /* Disable write protection */
  Device->SDCR[Bank] &= ~FMC_SDRAM_WRITE_PROTECTION_ENABLE;
}
  
/**
  * @brief  Send Command to the FMC SDRAM bank
  * @param  Device: Pointer to SDRAM device instance
  * @param  Command: Pointer to SDRAM command structure   
  * @param  Timing: Pointer to SDRAM Timing structure
  * @param  Timeout: Timeout wait value
  * @retval HAL state
  */  
unsigned int FMC_SDRAM_SendCommand(FMC_SDRAM_TypeDef *Device, FMC_SDRAM_CommandTypeDef *Command, unsigned int Timeout)
{
  __IO unsigned int tmpr;
  unsigned int tickstart;
  
  /* Set command register */
  tmpr = (unsigned int)((Command->CommandMode)                  |\
                    (Command->CommandTarget)                |\
                    (((Command->AutoRefreshNumber)-1) << 5) |\
                    ((Command->ModeRegisterDefinition) << 9)
                    );
    
  Device->SDCMR = tmpr;

  /* Get tick */ 
  tickstart = 0;

  /* wait until command is send */
  while(Device->SDSR & FMC_SDSR_BUSY)
  {
    /* Check for the Timeout */
    if(Timeout != _MAX_DELAY)
    {
      if((Timeout == 0)||(tickstart++ > Timeout))
      {
        return 1;
      }
    }     
  }
  
  return 0;
}

/**
  * @brief  Program the SDRAM Memory Refresh rate.
  * @param  Device: Pointer to SDRAM device instance  
  * @param  RefreshRate: The SDRAM refresh rate value.       
  * @retval HAL state
  */
void FMC_SDRAM_ProgramRefreshRate(FMC_SDRAM_TypeDef *Device, unsigned int RefreshRate)
{
  /* Set the refresh rate in command register */
  Device->SDRTR |= (RefreshRate<<1);
}

/**
  * @brief  Set the Number of consecutive SDRAM Memory auto Refresh commands.
  * @param  Device: Pointer to SDRAM device instance  
  * @param  AutoRefreshNumber: Specifies the auto Refresh number.       
  * @retval None
  */
void FMC_SDRAM_SetAutoRefreshNumber(FMC_SDRAM_TypeDef *Device, unsigned int AutoRefreshNumber)
{
  /* Set the Auto-refresh number in command register */
  Device->SDCMR |= (AutoRefreshNumber << 5); 
}

/**
  * @brief  Returns the indicated FMC SDRAM bank mode status.
  * @param  Device: Pointer to SDRAM device instance  
  * @param  Bank: Defines the FMC SDRAM bank. This parameter can be 
  *                     FMC_Bank1_SDRAM or FMC_Bank2_SDRAM. 
  * @retval The FMC SDRAM bank mode status, could be on of the following values:
  *         FMC_SDRAM_NORMAL_MODE, FMC_SDRAM_SELF_REFRESH_MODE or 
  *         FMC_SDRAM_POWER_DOWN_MODE.           
  */
unsigned int FMC_SDRAM_GetModeStatus(FMC_SDRAM_TypeDef *Device, unsigned int Bank)
{
  unsigned int tmpreg = 0;
  
  /* Get the corresponding bank mode */
  if(Bank == FMC_SDRAM_BANK1)
  {
    tmpreg = (unsigned int)(Device->SDSR & FMC_SDSR_MODES1); 
  }
  else
  {
    tmpreg = ((unsigned int)(Device->SDSR & FMC_SDSR_MODES2) >> 2);
  }
  
  /* Return the mode status */
  return tmpreg;
}
