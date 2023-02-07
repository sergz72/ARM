#include <adc.h>

/* ADC CFGR mask */
#define CFGR1_CLEAR_MASK           ((uint32_t)0xFFFFD203)

/* Calibration time out */
#define CALIBRATION_TIMEOUT       ((uint32_t)0x0000F000)

/**
  * @brief  Enables or disables the specified ADC peripheral.
  * @param  ADCx: where x can be 1 to select the ADC1 peripheral.
  * @param  NewState: new state of the ADCx peripheral. 
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Set the ADEN bit to Enable the ADC peripheral */
    ADCx->CR |= (uint32_t)ADC_CR_ADEN;
  }
  else
  {
    /* Set the ADDIS to Disable the ADC peripheral */
    ADCx->CR |= (uint32_t)ADC_CR_ADDIS;
  }
}

/**
  * @brief  Initializes the ADCx peripheral according to the specified parameters
  *         in the ADC_InitStruct.
  * @note   This function is used to configure the global features of the ADC ( 
  *         Resolution, Data Alignment, continuous mode activation, External 
  *         trigger source and edge, Sequence Scan Direction).   
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @param  ADC_InitStruct: pointer to an ADC_InitTypeDef structure that contains 
  *         the configuration information for the specified ADC peripheral.
  * @retval None
  */
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct)
{
  uint32_t tmpreg = 0;

  /* Get the ADCx CFGR value */
  tmpreg = ADCx->CFGR1;

  /* Clear SCANDIR, RES[1:0], ALIGN, EXTSEL[2:0], EXTEN[1:0] and CONT bits */
  tmpreg &= CFGR1_CLEAR_MASK;

  /*---------------------------- ADCx CFGR Configuration ---------------------*/

  /* Set RES[1:0] bits according to ADC_Resolution value */
  /* Set CONT bit according to ADC_ContinuousConvMode value */
  /* Set EXTEN[1:0] bits according to ADC_ExternalTrigConvEdge value */
  /* Set EXTSEL[2:0] bits according to ADC_ExternalTrigConv value */
  /* Set ALIGN bit according to ADC_DataAlign value */
  /* Set SCANDIR bit according to ADC_ScanDirection value */
 
  tmpreg  |= (uint32_t)(ADC_InitStruct->ADC_Resolution | ((uint32_t)(ADC_InitStruct->ADC_ContinuousConvMode) << 13) |
             ADC_InitStruct->ADC_ExternalTrigConvEdge | ADC_InitStruct->ADC_ExternalTrigConv |
             ADC_InitStruct->ADC_DataAlign | ADC_InitStruct->ADC_ScanDirection);

  /* Write to ADCx CFGR */
  ADCx->CFGR1 = tmpreg;
}

/**
  * @brief  Configure the ADC to either be clocked by the asynchronous clock(which is
  *         independent, the dedicated 14MHz clock) or the synchronous clock derived from
  *         the APB clock of the ADC bus interface divided by 2 or 4
  * @note   This function can be called only when ADC is disabled.
  * @param  ADCx: where x can be 1 to select the ADC1 peripheral.
  * @param  ADC_ClockMode: This parameter can be :
  *            @arg ADC_ClockMode_AsynClk: ADC clocked by the dedicated 14MHz clock
  *            @arg ADC_ClockMode_SynClkDiv2: ADC clocked by PCLK/2
  *            @arg ADC_ClockMode_SynClkDiv4: ADC clocked by PCLK/4  
  * @retval None
  */
void ADC_ClockModeConfig(ADC_TypeDef* ADCx, uint32_t ADC_ClockMode)
{
    /* Configure the ADC Clock mode according to ADC_ClockMode */
    ADCx->CFGR2 = (uint32_t)ADC_ClockMode;
}

/**
  * @brief  Active the Calibration operation for the selected ADC.
  * @note   The Calibration can be initiated only when ADC is still in the 
  *         reset configuration (ADEN must be equal to 0).
  * @param  ADCx: where x can be 1 to select the ADC1 peripheral.
  * @retval ADC Calibration factor 
  */
uint32_t ADC_GetCalibrationFactor(ADC_TypeDef* ADCx)
{
  uint32_t tmpreg = 0, calibrationcounter = 0, calibrationstatus = 0;

  /* Set the ADC calibartion */
  ADCx->CR |= (uint32_t)ADC_CR_ADCAL;
  
  /* Wait until no ADC calibration is completed */
  do
  {
    calibrationstatus = ADCx->CR & ADC_CR_ADCAL;
    calibrationcounter++;  
  } while((calibrationcounter != CALIBRATION_TIMEOUT) && (calibrationstatus != 0x00));
    
  if((uint32_t)(ADCx->CR & ADC_CR_ADCAL) == RESET)
  {
    /*Get the calibration factor from the ADC data register */
    tmpreg = ADCx->DR;
  }
  else
  {
    /* Error factor */
    tmpreg = 0x00000000;
  }
  return tmpreg;
}

/**
  * @brief  Checks whether the specified ADC flag is set or not.
  * @param  ADCx: where x can be 1 to select the ADC1 peripheral.
  * @param  ADC_FLAG: specifies the flag to check. 
  *          This parameter can be one of the following values:
  *            @arg ADC_FLAG_AWD: Analog watchdog flag
  *            @arg ADC_FLAG_OVR: Overrun flag 
  *            @arg ADC_FLAG_EOSEQ: End of Sequence flag
  *            @arg ADC_FLAG_EOC: End of conversion flag
  *            @arg ADC_FLAG_EOSMP: End of sampling flag
  *            @arg ADC_FLAG_ADRDY: ADC Ready flag
  *            @arg ADC_FLAG_ADEN: ADC enable flag 
  *            @arg ADC_FLAG_ADDIS: ADC disable flag 
  *            @arg ADC_FLAG_ADSTART: ADC start flag 
  *            @arg ADC_FLAG_ADSTP: ADC stop flag
  *            @arg ADC_FLAG_ADCAL: ADC Calibration flag
  * @retval The new state of ADC_FLAG (SET or RESET).
  */
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint32_t ADC_FLAG)
{
  FlagStatus bitstatus = RESET;
  uint32_t tmpreg = 0;

  if((uint32_t)(ADC_FLAG & 0x01000000))
  {
    tmpreg = ADCx->CR & 0xFEFFFFFF;
  }
  else
  {
    tmpreg = ADCx->ISR;
  }
  
  /* Check the status of the specified ADC flag */
  if ((tmpreg & ADC_FLAG) != (uint32_t)RESET)
  {
    /* ADC_FLAG is set */
    bitstatus = SET;
  }
  else
  {
    /* ADC_FLAG is reset */
    bitstatus = RESET;
  }
  /* Return the ADC_FLAG status */
  return  bitstatus;
}

/**
  * @brief  Start Conversion for the selected ADC channels.
  * @note   In continuous mode, ADSTART is not cleared by hardware with the 
  *         assertion of EOSEQ because the sequence is automatic relaunched
  * @param  ADCx: where x can be 1 to select the ADC1 peripheral.
  * @retval None
  */
void ADC_StartOfConversion(ADC_TypeDef* ADCx)
{
  ADCx->CR |= (uint32_t)ADC_CR_ADSTART;
}

/**
  * @brief  Configures for the selected ADC and its sampling time.
  * @param  ADCx: where x can be 1 to select the ADC peripheral.
  * @param  ADC_Channel: the ADC channel to configure. 
  *          This parameter can be any combination of the following values:
  *            @arg ADC_Channel_0: ADC Channel0 selected
  *            @arg ADC_Channel_1: ADC Channel1 selected
  *            @arg ADC_Channel_2: ADC Channel2 selected
  *            @arg ADC_Channel_3: ADC Channel3 selected
  *            @arg ADC_Channel_4: ADC Channel4 selected
  *            @arg ADC_Channel_5: ADC Channel5 selected
  *            @arg ADC_Channel_6: ADC Channel6 selected
  *            @arg ADC_Channel_7: ADC Channel7 selected
  *            @arg ADC_Channel_8: ADC Channel8 selected
  *            @arg ADC_Channel_9: ADC Channel9 selected
  *            @arg ADC_Channel_10: ADC Channel10 selected, not available for STM32F031 devices
  *            @arg ADC_Channel_11: ADC Channel11 selected, not available for STM32F031 devices
  *            @arg ADC_Channel_12: ADC Channel12 selected, not available for STM32F031 devices
  *            @arg ADC_Channel_13: ADC Channel13 selected, not available for STM32F031 devices
  *            @arg ADC_Channel_14: ADC Channel14 selected, not available for STM32F031 devices
  *            @arg ADC_Channel_15: ADC Channel15 selected, not available for STM32F031 devices
  *            @arg ADC_Channel_16: ADC Channel16 selected
  *            @arg ADC_Channel_17: ADC Channel17 selected
  *            @arg ADC_Channel_18: ADC Channel18 selected, not available for STM32F030 devices
  * @param  ADC_SampleTime: The sample time value to be set for the selected channel. 
  *          This parameter can be one of the following values:
  *            @arg ADC_SampleTime_1_5Cycles: Sample time equal to 1.5 cycles  
  *            @arg ADC_SampleTime_7_5Cycles: Sample time equal to 7.5 cycles
  *            @arg ADC_SampleTime_13_5Cycles: Sample time equal to 13.5 cycles
  *            @arg ADC_SampleTime_28_5Cycles: Sample time equal to 28.5 cycles
  *            @arg ADC_SampleTime_41_5Cycles: Sample time equal to 41.5 cycles
  *            @arg ADC_SampleTime_55_5Cycles: Sample time equal to 55.5 cycles
  *            @arg ADC_SampleTime_71_5Cycles: Sample time equal to 71.5 cycles
  *            @arg ADC_SampleTime_239_5Cycles: Sample time equal to 239.5 cycles
  * @retval None
  */
void ADC_ChannelConfig(ADC_TypeDef* ADCx, uint32_t ADC_Channel, uint32_t ADC_SampleTime)
{
  /* Configure the ADC Channel */
  ADCx->CHSELR = ADC_Channel;
  ADCx->SMPR = ADC_SampleTime;
}

/**
  * @brief  Returns the last ADCx conversion result data for ADC channel.  
  * @param  ADCx: where x can be 1 to select the ADC1 peripheral.
  * @retval The Data conversion value.
  */
unsigned int ADC_GetConversionValue(ADC_TypeDef* ADCx)
{
  /* Return the selected ADC conversion value */
  return ADCx->DR & 0xFFF;
}
