#include <dma.h>

#define HAL_TIMEOUT_DMA_ABORT    0xFFFFF

static unsigned int DMA_CheckFifoParam(DMA_HandleTypeDef *hdma)
{
  int status = 0;
  uint32_t tmp = hdma->Init.FIFOThreshold;

  /* Memory Data size equal to Byte */
  if(hdma->Init.MemDataAlignment == DMA_MDATAALIGN_BYTE)
  {
    switch (tmp)
    {
      case DMA_FIFO_THRESHOLD_1QUARTERFULL:
      case DMA_FIFO_THRESHOLD_3QUARTERSFULL:
        if ((hdma->Init.MemBurst & DMA_SxCR_MBURST_1) == DMA_SxCR_MBURST_1)
        {
          status = 1;
        }
        break;
      case DMA_FIFO_THRESHOLD_HALFFULL:
        if (hdma->Init.MemBurst == DMA_MBURST_INC16)
        {
          status = 1;
        }
        break;
      case DMA_FIFO_THRESHOLD_FULL:
        break;
      default:
        break;
    }
  }
  /* Memory Data size equal to Half-Word */
  else if (hdma->Init.MemDataAlignment == DMA_MDATAALIGN_HALFWORD)
  {
    switch (tmp)
    {
      case DMA_FIFO_THRESHOLD_1QUARTERFULL:
      case DMA_FIFO_THRESHOLD_3QUARTERSFULL:
        status = 1;
        break;
      case DMA_FIFO_THRESHOLD_HALFFULL:
        if ((hdma->Init.MemBurst & DMA_SxCR_MBURST_1) == DMA_SxCR_MBURST_1)
        {
          status = 1;
        }
        break;
      case DMA_FIFO_THRESHOLD_FULL:
        if (hdma->Init.MemBurst == DMA_MBURST_INC16)
        {
          status = 1;
        }
        break;
      default:
        break;
    }
  }
  /* Memory Data size equal to Word */
  else
  {
    switch (tmp)
    {
      case DMA_FIFO_THRESHOLD_1QUARTERFULL:
      case DMA_FIFO_THRESHOLD_HALFFULL:
      case DMA_FIFO_THRESHOLD_3QUARTERSFULL:
        status = 1;
        break;
      case DMA_FIFO_THRESHOLD_FULL:
        if ((hdma->Init.MemBurst & DMA_SxCR_MBURST_1) == DMA_SxCR_MBURST_1)
        {
          status = 1;
        }
        break;
      default:
        break;
    }
  }

  return status;
}

/**
  * @brief  Returns the DMA Stream base address depending on stream number
  * @param  hdma       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.
  * @retval Stream base address
  */
static unsigned int DMA_CalcBaseAndBitshift(DMA_HandleTypeDef *hdma)
{
  unsigned int stream_number = (((unsigned int)hdma->Instance & 0xFFU) - 16U) / 24U;

  /* lookup table for necessary bitshift of flags within status registers */
  static const uint8_t flagBitshiftOffset[8U] = {0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U};
  hdma->StreamIndex = flagBitshiftOffset[stream_number];

  if (stream_number > 3U)
  {
    /* return pointer to HISR and HIFCR */
    hdma->StreamBaseAddress = (((uint32_t)hdma->Instance & (uint32_t)(~0x3FFU)) + 4U);
  }
  else
  {
    /* return pointer to LISR and LIFCR */
    hdma->StreamBaseAddress = ((uint32_t)hdma->Instance & (uint32_t)(~0x3FFU));
  }

  return hdma->StreamBaseAddress;
}

unsigned int DMA_Init(DMA_HandleTypeDef *hdma)
{
  unsigned int tmp;
  DMA_Base_Registers *regs;
  unsigned int counter = 0;

  /* Disable the peripheral */
  DMA_DISABLE(hdma);
  
  /* Check if the DMA Stream is effectively disabled */
  while((hdma->Instance->CR & DMA_SxCR_EN))
  {
    /* Check for the Timeout */
    if(counter++ > HAL_TIMEOUT_DMA_ABORT)
    {
      return 1;
    }
  }
  
  /* Get the CR register value */
  tmp = hdma->Instance->CR;

  /* Clear CHSEL, MBURST, PBURST, PL, MSIZE, PSIZE, MINC, PINC, CIRC, DIR, CT and DBM bits */
  tmp &= ((unsigned int)~(DMA_SxCR_CHSEL | DMA_SxCR_MBURST | DMA_SxCR_PBURST |
                      DMA_SxCR_PL    | DMA_SxCR_MSIZE  | DMA_SxCR_PSIZE  |
                      DMA_SxCR_MINC  | DMA_SxCR_PINC   | DMA_SxCR_CIRC   |
                      DMA_SxCR_DIR   | DMA_SxCR_CT     | DMA_SxCR_DBM));

  /* Prepare the DMA Stream configuration */
  tmp |= hdma->Init.Channel             | hdma->Init.Direction        |
         hdma->Init.PeriphInc           | hdma->Init.MemInc           |
         hdma->Init.PeriphDataAlignment | hdma->Init.MemDataAlignment |
         hdma->Init.Mode                | hdma->Init.Priority;

  /* the Memory burst and peripheral burst are not used when the FIFO is disabled */
  if(hdma->Init.FIFOMode == DMA_FIFOMODE_ENABLE)
  {
    /* Get memory burst and peripheral burst */
    tmp |= hdma->Init.MemBurst | hdma->Init.PeriphBurst;
  }
  
  /* Write to DMA Stream CR register */
  hdma->Instance->CR = tmp;  

  /* Get the FCR register value */
  tmp = hdma->Instance->FCR;

  /* Clear Direct mode and FIFO threshold bits */
  tmp &= (unsigned int)~(DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);

  /* Prepare the DMA Stream FIFO configuration */
  tmp |=hdma->Init.FIFOMode;

  /* The FIFO threshold is not used when the FIFO mode is disabled */
  if(hdma->Init.FIFOMode == DMA_FIFOMODE_ENABLE)
  {
    /* Get the FIFO threshold */
    tmp |=hdma->Init.FIFOThreshold;
    
    /* Check compatibility between FIFO threshold level and size of the memory burst */
    /* for INCR4, INCR8, INCR16 bursts */
    if (hdma->Init.MemBurst != DMA_MBURST_SINGLE)
    {
      if (DMA_CheckFifoParam(hdma))
      {
        return 1; 
      }
    }
  }
  
  /* Write to DMA Stream FCR */
  hdma->Instance->FCR = tmp;

  /* Initialize StreamBaseAddress and StreamIndex parameters to be used to calculate
     DMA steam Base Address needed by HAL_DMA_IRQHandler() and HAL_DMA_PollForTransfer() */
  regs = (DMA_Base_Registers *)DMA_CalcBaseAndBitshift(hdma);
  
  /* Clear all interrupt flags */
  regs->IFCR = 0x3FU << hdma->StreamIndex;

  return 0;
}

/**
  * @brief  Sets the DMA Transfer parameter.
  * @param  hdma       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.
  * @param  SrcAddress The source memory Buffer address
  * @param  DstAddress The destination memory Buffer address
  * @param  DataLength The length of data to be transferred from source to destination
  * @retval HAL status
  */
static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DstAddress2, uint32_t DataLength)
{
  if (!DstAddress2)
  {
    /* Clear DBM bit */
    hdma->Instance->CR &= (uint32_t) (~DMA_SxCR_DBM);
  }
  else
  {
    /* Set DBM bit */
    hdma->Instance->CR |= DMA_SxCR_DBM;
  }

  /* Configure DMA Stream data length */
  hdma->Instance->NDTR = DataLength;

  /* Memory to Peripheral */
  if((hdma->Init.Direction) == DMA_MEMORY_TO_PERIPH)
  {
    /* Configure DMA Stream destination address */
    hdma->Instance->PAR = DstAddress;

    /* Configure DMA Stream source address */
    hdma->Instance->M0AR = SrcAddress;
  }
    /* Peripheral to Memory */
  else
  {
    /* Configure DMA Stream source address */
    hdma->Instance->PAR = SrcAddress;

    /* Configure DMA Stream destination address */
    hdma->Instance->M0AR = DstAddress;
    if (DstAddress2)
      hdma->Instance->M1AR = DstAddress2;
  }
}

/**
  * @brief  Starts the DMA Transfer.
  * @param  hdma       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.
  * @param  SrcAddress The source memory Buffer address
  * @param  DstAddress The destination memory Buffer address
  * @param  DataLength The length of data to be transferred from source to destination
  * @retval HAL status
  */
void DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DstAddress2, uint32_t DataLength)
{
  /* Configure the source, destination address and the data length */
  DMA_SetConfig(hdma, SrcAddress, DstAddress, DstAddress2, DataLength);
  /* Enable the Peripheral */
  DMA_ENABLE(hdma);
}

/**
  * @brief  Start the DMA Transfer with interrupt enabled.
  * @param  hdma       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Stream.
  * @param  SrcAddress The source memory Buffer address
  * @param  DstAddress The destination memory Buffer address
  * @param  DataLength The length of data to be transferred from source to destination
  * @retval HAL status
  */
void DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DstAddress2, uint32_t DataLength)
{
  /* calculate DMA base and stream number */
  DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;
  /* Configure the source, destination address and the data length */
  DMA_SetConfig(hdma, SrcAddress, DstAddress, DstAddress2, DataLength);

  /* Clear all interrupt flags at correct offset within the register */
  regs->IFCR = 0x3FU << hdma->StreamIndex;

  /* Enable Common interrupts*/
  hdma->Instance->CR  |= DMA_IT_TC | DMA_IT_TE | DMA_IT_DME;
  hdma->Instance->FCR |= DMA_IT_FE;

  /* Enable the Peripheral */
  DMA_ENABLE(hdma);
}
