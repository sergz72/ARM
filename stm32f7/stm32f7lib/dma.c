#include <dma.h>

#define HAL_TIMEOUT_DMA_ABORT    0xFFFFF

static unsigned int DMA_CheckFifoParam(DMA_HandleTypeDef *hdma)
{
  int status = 0;
  unsigned int tmp = hdma->Init.FIFOThreshold;

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
    hdma->StreamBaseAddress = (((unsigned int)hdma->Instance & (unsigned int)(~0x3FFU)) + 4U);
  }
  else
  {
    /* return pointer to LISR and LIFCR */
    hdma->StreamBaseAddress = ((unsigned int)hdma->Instance & (unsigned int)(~0x3FFU));
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
static void DMA_SetConfig(DMA_HandleTypeDef *hdma, unsigned int SrcAddress, unsigned int DstAddress, unsigned int DstAddress2, unsigned int DataLength)
{
  if (!DstAddress2)
  {
    /* Clear DBM bit */
    hdma->Instance->CR &= (unsigned int) (~DMA_SxCR_DBM);
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
void DMA_Start(DMA_HandleTypeDef *hdma, unsigned int SrcAddress, unsigned int DstAddress, unsigned int DstAddress2, unsigned int DataLength)
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
void DMA_Start_IT(DMA_HandleTypeDef *hdma, unsigned int SrcAddress, unsigned int DstAddress, unsigned int DstAddress2, unsigned int DataLength)
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

/**
  * @brief  Handles DMA interrupt request.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Stream.
  * @retval None
  */
void DMA_IRQHandler(DMA_HandleTypeDef *hdma)
{
  unsigned int tmpisr;
  __IO unsigned int count = 0;
  unsigned int timeout = SYSCLK_FREQ / 9600;

  /* calculate DMA base and stream number */
  DMA_Base_Registers *regs = (DMA_Base_Registers *)hdma->StreamBaseAddress;

  tmpisr = regs->ISR;

  /* Transfer Error Interrupt management ***************************************/
  if ((tmpisr & (DMA_FLAG_TEIF0_4 << hdma->StreamIndex)) != RESET)
  {
    if(DMA_GET_IT_SOURCE(hdma, DMA_IT_TE) != RESET)
    {
      /* Disable the transfer error interrupt */
      hdma->Instance->CR  &= ~(DMA_IT_TE);

      /* Clear the transfer error flag */
      regs->IFCR = DMA_FLAG_TEIF0_4 << hdma->StreamIndex;

      /* Update error code */
      hdma->ErrorCode |= DMA_ERROR_TE;
    }
  }
  /* FIFO Error Interrupt management ******************************************/
  if ((tmpisr & (DMA_FLAG_FEIF0_4 << hdma->StreamIndex)) != RESET)
  {
    if(DMA_GET_IT_SOURCE(hdma, DMA_IT_FE) != RESET)
    {
      /* Clear the FIFO error flag */
      regs->IFCR = DMA_FLAG_FEIF0_4 << hdma->StreamIndex;

      /* Update error code */
      hdma->ErrorCode |= DMA_ERROR_FE;
    }
  }
  /* Direct Mode Error Interrupt management ***********************************/
  if ((tmpisr & (DMA_FLAG_DMEIF0_4 << hdma->StreamIndex)) != RESET)
  {
    if(DMA_GET_IT_SOURCE(hdma, DMA_IT_DME) != RESET)
    {
      /* Clear the direct mode error flag */
      regs->IFCR = DMA_FLAG_DMEIF0_4 << hdma->StreamIndex;

      /* Update error code */
      hdma->ErrorCode |= DMA_ERROR_DME;
    }
  }
  /* Half Transfer Complete Interrupt management ******************************/
  if ((tmpisr & (DMA_FLAG_HTIF0_4 << hdma->StreamIndex)) != RESET)
  {
    if(DMA_GET_IT_SOURCE(hdma, DMA_IT_HT) != RESET)
    {
      /* Clear the half transfer complete flag */
      regs->IFCR = DMA_FLAG_HTIF0_4 << hdma->StreamIndex;

      /* Multi_Buffering mode enabled */
      if(((hdma->Instance->CR) & (unsigned int)(DMA_SxCR_DBM)) != RESET)
      {
        /* Current memory buffer used is Memory 0 */
        if((hdma->Instance->CR & DMA_SxCR_CT) == RESET)
        {
          if(hdma->XferHalfCpltCallback != NULL)
          {
            /* Half transfer callback */
            hdma->XferHalfCpltCallback(hdma);
          }
        }
          /* Current memory buffer used is Memory 1 */
        else
        {
          if(hdma->XferM1HalfCpltCallback != NULL)
          {
            /* Half transfer callback */
            hdma->XferM1HalfCpltCallback(hdma);
          }
        }
      }
      else
      {
        /* Disable the half transfer interrupt if the DMA mode is not CIRCULAR */
        if((hdma->Instance->CR & DMA_SxCR_CIRC) == RESET)
        {
          /* Disable the half transfer interrupt */
          hdma->Instance->CR  &= ~(DMA_IT_HT);
        }

        if(hdma->XferHalfCpltCallback != NULL)
        {
          /* Half transfer callback */
          hdma->XferHalfCpltCallback(hdma);
        }
      }
    }
  }
  /* Transfer Complete Interrupt management ***********************************/
  if ((tmpisr & (DMA_FLAG_TCIF0_4 << hdma->StreamIndex)) != RESET)
  {
    if(DMA_GET_IT_SOURCE(hdma, DMA_IT_TC) != RESET)
    {
      /* Clear the transfer complete flag */
      regs->IFCR = DMA_FLAG_TCIF0_4 << hdma->StreamIndex;

      if(DMA_STATE_ABORT == hdma->State)
      {
        /* Disable all the transfer interrupts */
        hdma->Instance->CR  &= ~(DMA_IT_TC | DMA_IT_TE | DMA_IT_DME);
        hdma->Instance->FCR &= ~(DMA_IT_FE);

        if((hdma->XferHalfCpltCallback != NULL) || (hdma->XferM1HalfCpltCallback != NULL))
        {
          hdma->Instance->CR  &= ~(DMA_IT_HT);
        }

        /* Clear all interrupt flags at correct offset within the register */
        regs->IFCR = 0x3FU << hdma->StreamIndex;

        /* Change the DMA state */
        hdma->State = DMA_STATE_READY;

        if(hdma->XferAbortCallback != NULL)
        {
          hdma->XferAbortCallback(hdma);
        }
        return;
      }

      if(((hdma->Instance->CR) & (unsigned int)(DMA_SxCR_DBM)) != RESET)
      {
        /* Current memory buffer used is Memory 0 */
        if((hdma->Instance->CR & DMA_SxCR_CT) == RESET)
        {
          if(hdma->XferM1CpltCallback != NULL)
          {
            /* Transfer complete Callback for memory1 */
            hdma->XferM1CpltCallback(hdma);
          }
        }
          /* Current memory buffer used is Memory 1 */
        else
        {
          if(hdma->XferCpltCallback != NULL)
          {
            /* Transfer complete Callback for memory0 */
            hdma->XferCpltCallback(hdma);
          }
        }
      }
        /* Disable the transfer complete interrupt if the DMA mode is not CIRCULAR */
      else
      {
        if((hdma->Instance->CR & DMA_SxCR_CIRC) == RESET)
        {
          /* Disable the transfer complete interrupt */
          hdma->Instance->CR  &= ~(DMA_IT_TC);

          /* Change the DMA state */
          hdma->State = DMA_STATE_READY;
        }

        if(hdma->XferCpltCallback != NULL)
        {
          /* Transfer complete callback */
          hdma->XferCpltCallback(hdma);
        }
      }
    }
  }

  /* manage error case */
  if(hdma->ErrorCode != DMA_ERROR_NONE)
  {
    if((hdma->ErrorCode & DMA_ERROR_TE) != RESET)
    {
      hdma->State = DMA_STATE_ABORT;

      /* Disable the stream */
      DMA_DISABLE(hdma);

      do
      {
        if (++count > timeout)
        {
          break;
        }
      }
      while((hdma->Instance->CR & DMA_SxCR_EN) != RESET);

      /* Change the DMA state */
      hdma->State = DMA_STATE_READY;
    }

    if(hdma->XferErrorCallback != NULL)
    {
      /* Transfer error callback */
      hdma->XferErrorCallback(hdma);
    }
  }
}
