#include "board.h"
#include "dac.h"

/* CR register Mask */
#define CR_CLEAR_MASK              ((uint32_t)0x00000FFE)

/* DAC Dual Channels SWTRIG masks */
#define DUAL_SWTRIG_SET            ((uint32_t)0x00000003)
#define DUAL_SWTRIG_RESET          ((uint32_t)0xFFFFFFFC)

/* DOR register offset */
#define DOR_OFFSET                 ((uint32_t)0x0000002C)

/**
  * @brief  Initializes the DAC peripheral according to the specified 
  *         parameters in the DAC_InitStruct.
  * @param  DACx: where x can be 1 or 2 to select the DAC peripheral.  
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_InitStruct: pointer to a DAC_InitTypeDef structure that
  *         contains the configuration information for the specified DAC channel.
  * @retval None
  */
void DAC_Init(DAC_TypeDef* DACx, uint32_t DAC_Channel, DAC_InitTypeDef* DAC_InitStruct)
{
  uint32_t tmpreg1 = 0, tmpreg2 = 0;

/*---------------------------- DAC CR Configuration --------------------------*/
  /* Get the DAC CR value */
  tmpreg1 = DACx->CR;
  /* Clear BOFFx, TENx, TSELx, WAVEx and MAMPx bits */
  tmpreg1 &= ~(CR_CLEAR_MASK << DAC_Channel);
  /* Configure for the selected DAC channel: buffer output, trigger, wave generation,
     mask/amplitude for wave generation */
  
  /* Set TSELx and TENx bits according to DAC_Trigger value */
  /* Set WAVEx bits according to DAC_WaveGeneration value */
  /* Set MAMPx bits according to DAC_LFSRUnmask_TriangleAmplitude value */ 
  /* Set BOFFx OUTENx bit according to DAC_Buffer_Switch value */   
  tmpreg2 = (DAC_InitStruct->DAC_Trigger | DAC_InitStruct->DAC_WaveGeneration |
             DAC_InitStruct->DAC_LFSRUnmask_TriangleAmplitude | DAC_InitStruct->DAC_Buffer_Switch);   
                   
  /* Calculate CR register value depending on DAC_Channel */
  tmpreg1 |= tmpreg2 << DAC_Channel;
  /* Write to DAC CR */
  DACx->CR = tmpreg1;
}

/**
  * @brief  Fills each DAC_InitStruct member with its default value.
  * @param  DAC_InitStruct: pointer to a DAC_InitTypeDef structure which will 
  *         be initialized.
  * @retval None
  */
void DAC_StructInit(DAC_InitTypeDef* DAC_InitStruct)
{
/*--------------- Reset DAC init structure parameters values -----------------*/
  /* Initialize the DAC_Trigger member */
  DAC_InitStruct->DAC_Trigger = DAC_Trigger_None;
  /* Initialize the DAC_WaveGeneration member */
  DAC_InitStruct->DAC_WaveGeneration = DAC_WaveGeneration_None;
  /* Initialize the DAC_LFSRUnmask_TriangleAmplitude member */
  DAC_InitStruct->DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
  /* Initialize the DAC_Buffer_Switch member */
  DAC_InitStruct->DAC_Buffer_Switch = DAC_BufferSwitch_Enable;
}

/**
  * @brief  Enables or disables the specified DAC channel.
  * @param  DACx: where x can be 1 or 2 to select the DAC peripheral.  
  * @param  DAC_Channel: The selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  NewState: new state of the DAC channel. 
  *          This parameter can be: ENABLE or DISABLE.
  * @note   When the DAC channel is enabled the trigger source can no more
  *         be modified.
  * @retval None
  */
void DAC_Cmd(DAC_TypeDef* DACx, uint32_t DAC_Channel, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the selected DAC channel */
    DACx->CR |= (DAC_CR_EN1 << DAC_Channel);
  }
  else
  {
    /* Disable the selected DAC channel */
    DACx->CR &= (~(DAC_CR_EN1 << DAC_Channel));
  }
}

/**
  * @brief  Enables or disables the selected DAC channel software trigger.
  * @param  DACx: where x can be 1 or 2 to select the DAC peripheral.  
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  NewState: new state of the selected DAC channel software trigger.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DAC_SoftwareTriggerCmd(DAC_TypeDef* DACx, uint32_t DAC_Channel, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable software trigger for the selected DAC channel */
    DACx->SWTRIGR |= (uint32_t)DAC_SWTRIGR_SWTRIG1 << (DAC_Channel >> 4);
  }
  else
  {
    /* Disable software trigger for the selected DAC channel */
    DACx->SWTRIGR &= ~((uint32_t)DAC_SWTRIGR_SWTRIG1 << (DAC_Channel >> 4));
  }
}

/**
  * @brief  Enables or disables simultaneously the two DAC channels software
  *         triggers.
  * @param  DACx: where x can be 1 to select the DAC1 peripheral.
  * @note   Dual trigger is not applicable for DAC2 (DAC2 integrates one channel).
  * @param  NewState: new state of the DAC channels software triggers.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DAC_DualSoftwareTriggerCmd(DAC_TypeDef* DACx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable software trigger for both DAC channels */
    DACx->SWTRIGR |= DUAL_SWTRIG_SET;
  }
  else
  {
    /* Disable software trigger for both DAC channels */
    DACx->SWTRIGR &= DUAL_SWTRIG_RESET;
  }
}

/**
  * @brief  Enables or disables the selected DAC channel wave generation.
  * @param  DACx: where x can be 1 to select the DAC1 peripheral.
  * @note   Wave generation is not available in DAC2.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_Wave: Specifies the wave type to enable or disable.
  *          This parameter can be one of the following values:
  *            @arg DAC_Wave_Noise: noise wave generation
  *            @arg DAC_Wave_Triangle: triangle wave generation
  * @param  NewState: new state of the selected DAC channel wave generation.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   
  * @retval None
  */
void DAC_WaveGenerationCmd(DAC_TypeDef* DACx, uint32_t DAC_Channel, uint32_t DAC_Wave, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the selected wave generation for the selected DAC channel */
    DACx->CR |= DAC_Wave << DAC_Channel;
  }
  else
  {
    /* Disable the selected wave generation for the selected DAC channel */
    DACx->CR &= ~(DAC_Wave << DAC_Channel);
  }
}

/**
  * @brief  Set the specified data holding register value for DAC channel1.
  * @param  DACx: where x can be 1 or 2 to select the DAC peripheral.  
  * @param  DAC_Align: Specifies the data alignment for DAC channel1.
  *          This parameter can be one of the following values:
  *            @arg DAC_Align_8b_R: 8bit right data alignment selected
  *            @arg DAC_Align_12b_L: 12bit left data alignment selected
  *            @arg DAC_Align_12b_R: 12bit right data alignment selected
  * @param  Data: Data to be loaded in the selected data holding register.
  * @retval None
  */
void DAC_SetChannel1Data(DAC_TypeDef* DACx, uint32_t DAC_Align, uint16_t Data)
{  
  __IO uint32_t tmp = 0;
  
  tmp = (uint32_t)DACx; 
  tmp += DHR12R1_OFFSET + DAC_Align;

  /* Set the DAC channel1 selected data holding register */
  *(__IO uint32_t *) tmp = Data;
}

/**
  * @brief  Set the specified data holding register value for DAC channel2.
  * @param  DACx: where x can be 1 to select the DAC peripheral.
  * @note   This function is available only for DAC1.
  * @param  DAC_Align: Specifies the data alignment for DAC channel2.
  *          This parameter can be one of the following values:
  *            @arg DAC_Align_8b_R: 8bit right data alignment selected
  *            @arg DAC_Align_12b_L: 12bit left data alignment selected
  *            @arg DAC_Align_12b_R: 12bit right data alignment selected
  * @param  Data : Data to be loaded in the selected data holding register.
  * @retval None
  */
void DAC_SetChannel2Data(DAC_TypeDef* DACx, uint32_t DAC_Align, uint16_t Data)
{
  __IO uint32_t tmp = 0;

  tmp = (uint32_t)DACx;
  tmp += DHR12R2_OFFSET + DAC_Align;

  /* Set the DAC channel2 selected data holding register */
  *(__IO uint32_t *)tmp = Data;
}

/**
  * @brief  Set the specified data holding register value for dual channel DAC.
  * @param  DACx: where x can be 1 to select the DAC peripheral.
  * @note   This function isn't applicable for DAC2.
  * @param  DAC_Align: Specifies the data alignment for dual channel DAC.
  *          This parameter can be one of the following values:
  *            @arg DAC_Align_8b_R: 8bit right data alignment selected
  *            @arg DAC_Align_12b_L: 12bit left data alignment selected
  *            @arg DAC_Align_12b_R: 12bit right data alignment selected
  * @param  Data2: Data for DAC Channel2 to be loaded in the selected data 
  *         holding register.
  * @param  Data1: Data for DAC Channel1 to be loaded in the selected data 
  *         holding register.
  * @note In dual mode, a unique register access is required to write in both
  *       DAC channels at the same time.
  * @retval None
  */
void DAC_SetDualChannelData(DAC_TypeDef* DACx, uint32_t DAC_Align, uint16_t Data2, uint16_t Data1)
{
  uint32_t data = 0, tmp = 0;
  
  /* Calculate and set dual DAC data holding register value */
  if (DAC_Align == DAC_Align_8b_R)
  {
    data = ((uint32_t)Data2 << 8) | Data1; 
  }
  else
  {
    data = ((uint32_t)Data2 << 16) | Data1;
  }
  
  tmp = (uint32_t)DACx;
  tmp += DHR12RD_OFFSET + DAC_Align;

  /* Set the dual DAC selected data holding register */
  *(__IO uint32_t *)tmp = data;
}

/**
  * @brief  Returns the last data output value of the selected DAC channel.
  * @param  DACx: where x can be 1 or 2 to select the DAC peripheral.  
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @retval The selected DAC channel data output value.
  */
uint16_t DAC_GetDataOutputValue(DAC_TypeDef* DACx, uint32_t DAC_Channel)
{
  __IO uint32_t tmp = 0;
  
  tmp = (uint32_t) DACx;
  tmp += DOR_OFFSET + ((uint32_t)DAC_Channel >> 2);
  
  /* Returns the DAC channel data output register value */
  return (uint16_t) (*(__IO uint32_t*) tmp);
}

/**
  * @brief  Enables or disables the specified DAC channel DMA request.
  *         When enabled DMA1 is generated when an external trigger (EXTI Line9,
  *         TIM2, TIM4, TIM6, TIM7 or TIM9  but not a software trigger) occurs
  * @param  DACx: where x can be 1 or 2 to select the DAC peripheral.
  * @param  DAC_Channel: the selected DAC channel.
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  NewState: new state of the selected DAC channel DMA request.
  *          This parameter can be: ENABLE or DISABLE.
  * @note The DAC channel1 (channel2) is mapped on DMA1 channel3 (channel4) which 
  *       must be already configured. 
  * @retval None
  */
void DAC_DMACmd(DAC_TypeDef* DACx, uint32_t DAC_Channel, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the selected DAC channel DMA request */
    DACx->CR |= (DAC_CR_DMAEN1 << DAC_Channel);
  }
  else
  {
    /* Disable the selected DAC channel DMA request */
    DACx->CR &= (~(DAC_CR_DMAEN1 << DAC_Channel));
  }
}

/**
  * @brief  Enables or disables the specified DAC interrupts.
  * @param  DACx: where x can be 1 or 2 to select the DAC peripheral.  
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_IT: specifies the DAC interrupt sources to be enabled or disabled. 
  *          This parameter can be:
  *            @arg DAC_IT_DMAUDR: DMA underrun interrupt mask
  * @note   The DMA underrun occurs when a second external trigger arrives before
  *         the acknowledgement for the first external trigger is received (first request).
  * @param  NewState: new state of the specified DAC interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */ 
void DAC_ITConfig(DAC_TypeDef* DACx, uint32_t DAC_Channel, uint32_t DAC_IT, FunctionalState NewState)  
{
  if (NewState != DISABLE)
  {
    /* Enable the selected DAC interrupts */
    DACx->CR |=  (DAC_IT << DAC_Channel);
  }
  else
  {
    /* Disable the selected DAC interrupts */
    DACx->CR &= (~(uint32_t)(DAC_IT << DAC_Channel));
  }
}

/**
  * @brief  Checks whether the specified DAC flag is set or not.
  * @param  DACx: where x can be 1 or 2 to select the DAC peripheral.  
  * @param  DAC_Channel: thee selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_FLAG: specifies the flag to check. 
  *          This parameter can be:
  *            @arg DAC_FLAG_DMAUDR: DMA underrun flag
  * @note   The DMA underrun occurs when a second external trigger arrives before
  *         the acknowledgement for the first external trigger is received (first request).
  * @retval The new state of DAC_FLAG (SET or RESET).
  */
FlagStatus DAC_GetFlagStatus(DAC_TypeDef* DACx, uint32_t DAC_Channel, uint32_t DAC_FLAG)
{
  FlagStatus bitstatus = RESET;

  /* Check the status of the specified DAC flag */
  if ((DACx->SR & (DAC_FLAG << DAC_Channel)) != (uint8_t)RESET)
  {
    /* DAC_FLAG is set */
    bitstatus = SET;
  }
  else
  {
    /* DAC_FLAG is reset */
    bitstatus = RESET;
  }
  /* Return the DAC_FLAG status */
  return  bitstatus;
}

/**
  * @brief  Clears the DAC channel's pending flags.
  * @param  DACx: where x can be 1 or 2 to select the DAC peripheral.  
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_FLAG: specifies the flag to clear. 
  *          This parameter can be:
  *            @arg DAC_FLAG_DMAUDR: DMA underrun flag                          
  * @retval None
  */
void DAC_ClearFlag(DAC_TypeDef* DACx, uint32_t DAC_Channel, uint32_t DAC_FLAG)
{
  /* Clear the selected DAC flags */
  DACx->SR = (DAC_FLAG << DAC_Channel);
}

/**
  * @brief  Checks whether the specified DAC interrupt has occurred or not.
  * @param  DACx: where x can be 1 or 2 to select the DAC peripheral.  
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_IT: specifies the DAC interrupt source to check. 
  *          This parameter can be:
  *            @arg DAC_IT_DMAUDR: DMA underrun interrupt mask
  * @note   The DMA underrun occurs when a second external trigger arrives before
  *         the acknowledgement for the first external trigger is received (first request).
  * @retval The new state of DAC_IT (SET or RESET).
  */
ITStatus DAC_GetITStatus(DAC_TypeDef* DACx, uint32_t DAC_Channel, uint32_t DAC_IT)
{
  ITStatus bitstatus = RESET;
  uint32_t enablestatus = 0;
  
  /* Get the DAC_IT enable bit status */
  enablestatus = (DACx->CR & (DAC_IT << DAC_Channel)) ;
  
  /* Check the status of the specified DAC interrupt */
  if (((DACx->SR & (DAC_IT << DAC_Channel)) != (uint32_t)RESET) && enablestatus)
  {
    /* DAC_IT is set */
    bitstatus = SET;
  }
  else
  {
    /* DAC_IT is reset */
    bitstatus = RESET;
  }
  /* Return the DAC_IT status */
  return  bitstatus;
}

/**
  * @brief  Clears the DAC channel's interrupt pending bits.
  * @param  DACx: where x can be 1 or 2 to select the DAC peripheral.
  * @param  DAC_Channel: the selected DAC channel. 
  *          This parameter can be one of the following values:
  *            @arg DAC_Channel_1: DAC Channel1 selected
  *            @arg DAC_Channel_2: DAC Channel2 selected
  * @param  DAC_IT: specifies the DAC interrupt pending bit to clear.
  *          This parameter can be the following values:
  *            @arg DAC_IT_DMAUDR: DMA underrun interrupt mask
  * @retval None
  */
void DAC_ClearITPendingBit(DAC_TypeDef* DACx, uint32_t DAC_Channel, uint32_t DAC_IT)
{
  /* Clear the selected DAC interrupt pending bits */
  DACx->SR = (DAC_IT << DAC_Channel);
}
