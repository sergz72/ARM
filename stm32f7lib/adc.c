#include <adc.h>

#define ADC_CR1_SCANCONV(_SCANCONV_MODE_) ((_SCANCONV_MODE_) << 8)
#define ADC_CR2_CONTINUOUS(_CONTINUOUS_MODE_) ((_CONTINUOUS_MODE_) << 1)
#define ADC_CR1_DISCONTINUOUS(_NBR_DISCONTINUOUSCONV_) (((_NBR_DISCONTINUOUSCONV_) - 1) << ADC_CR1_DISCNUM_Pos)
#define ADC_SQR1(_NbrOfConversion_) (((_NbrOfConversion_) - 1) << 20)
#define ADC_CR2_DMAContReq(_DMAContReq_MODE_) ((_DMAContReq_MODE_) << 9)
#define ADC_CR2_EOCSelection(_EOCSelection_MODE_) ((_EOCSelection_MODE_) << 10)

#define ADC_SMPR1(_SAMPLETIME_, _CHANNELNB_) ((_SAMPLETIME_) << (3 * (_CHANNELNB_ - 10)))
#define ADC_SMPR2(_SAMPLETIME_, _CHANNELNB_) ((_SAMPLETIME_) << (3 * _CHANNELNB_))
#define ADC_SQR3_RK(_CHANNELNB_, _RANKNB_) (_CHANNELNB_ << (5 * ((_RANKNB_) - 1)))
#define ADC_SQR2_RK(_CHANNELNB_, _RANKNB_) (_CHANNELNB_ << (5 * ((_RANKNB_) - 7)))
#define ADC_SQR1_RK(_CHANNELNB_, _RANKNB_) (_CHANNELNB_ << (5 * ((_RANKNB_) - 13)))

#define ADC_TEMPSENSOR_DELAY_US         10
#define ADC_STAB_DELAY_US               3

void ADC_Init(ADC_TypeDef *adc, ADC_InitTypeDef* init)
{
  /* Set ADC parameters */
  /* Set the ADC clock prescaler */
  ADC->CCR &= ~(ADC_CCR_ADCPRE);
  ADC->CCR |=  init->ClockPrescaler;
  
  /* Set ADC scan mode */
  adc->CR1 &= ~(ADC_CR1_SCAN);
  adc->CR1 |=  ADC_CR1_SCANCONV(init->ScanConvMode);
  
  /* Set ADC resolution */
  adc->CR1 &= ~(ADC_CR1_RES);
  adc->CR1 |=  init->Resolution;
  
  /* Set ADC data alignment */
  adc->CR2 &= ~(ADC_CR2_ALIGN);
  adc->CR2 |= init->DataAlign;
  
  /* Enable external trigger if trigger selection is different of software  */
  /* start.                                                                 */
  /* Note: This configuration keeps the hardware feature of parameter       */
  /*       ExternalTrigConvEdge "trigger edge none" equivalent to           */
  /*       software start.                                                  */
  if(init->ExternalTrigConv != ADC_SOFTWARE_START)
  {
    /* Select external trigger to start conversion */
    adc->CR2 &= ~(ADC_CR2_EXTSEL);
    adc->CR2 |= init->ExternalTrigConv;
    
    /* Select external trigger polarity */
    adc->CR2 &= ~(ADC_CR2_EXTEN);
    adc->CR2 |= init->ExternalTrigConvEdge;
  }
  else
  {
    /* Reset the external trigger */
    adc->CR2 &= ~(ADC_CR2_EXTSEL);
    adc->CR2 &= ~(ADC_CR2_EXTEN);
  }
  
  /* Enable or disable ADC continuous conversion mode */
  adc->CR2 &= ~(ADC_CR2_CONT);
  adc->CR2 |= ADC_CR2_CONTINUOUS((unsigned int)init->ContinuousConvMode);
  
  if(init->DiscontinuousConvMode != DISABLE)
  {
    /* Enable the selected ADC regular discontinuous mode */
    adc->CR1 |= (unsigned int)ADC_CR1_DISCEN;
    
    /* Set the number of channels to be converted in discontinuous mode */
    adc->CR1 &= ~(ADC_CR1_DISCNUM);
    adc->CR1 |=  ADC_CR1_DISCONTINUOUS(init->NbrOfDiscConversion);
  }
  else
  {
    /* Disable the selected ADC regular discontinuous mode */
    adc->CR1 &= ~(ADC_CR1_DISCEN);
  }
  
  /* Set ADC number of conversion */
  adc->SQR1 &= ~(ADC_SQR1_L);
  adc->SQR1 |=  ADC_SQR1(init->NbrOfConversion);
  
  /* Enable or disable ADC DMA continuous request */
  adc->CR2 &= ~(ADC_CR2_DDS);
  adc->CR2 |= ADC_CR2_DMAContReq((unsigned int)init->DMAContinuousRequests);
  
  /* Enable or disable ADC end of conversion selection */
  adc->CR2 &= ~(ADC_CR2_EOCS);
  adc->CR2 |= ADC_CR2_EOCSelection(init->EOCSelection);
}

/**
* @brief  Configures for the selected ADC regular channel its corresponding
*         rank in the sequencer and its sample time.
* @retval void
*/
void ADC_ConfigChannel(ADC_TypeDef *adc, ADC_ChannelConfTypeDef* sConfig)
{
  __IO unsigned int counter = 0;

  /* if ADC_Channel_10 ... ADC_Channel_18 is selected */
  if (sConfig->Channel > ADC_CHANNEL_9)
  {
    /* Clear the old sample time */
    adc->SMPR1 &= ~ADC_SMPR1(ADC_SMPR1_SMP10, sConfig->Channel);

    if (sConfig->Channel == ADC_CHANNEL_TEMPSENSOR)
    {
      /* Set the new sample time */
      adc->SMPR1 |= ADC_SMPR1(sConfig->SamplingTime, ADC_CHANNEL_18);
    }
    else
    {
      /* Set the new sample time */
      adc->SMPR1 |= ADC_SMPR1(sConfig->SamplingTime, sConfig->Channel);
    }
  }
  else /* ADC_Channel include in ADC_Channel_[0..9] */
  {
    /* Clear the old sample time */
    adc->SMPR2 &= ~ADC_SMPR2(ADC_SMPR2_SMP0, sConfig->Channel);

    /* Set the new sample time */
    adc->SMPR2 |= ADC_SMPR2(sConfig->SamplingTime, sConfig->Channel);
  }

  /* For Rank 1 to 6 */
  if (sConfig->Rank < 7)
  {
    /* Clear the old SQx bits for the selected rank */
    adc->SQR3 &= ~ADC_SQR3_RK(ADC_SQR3_SQ1, sConfig->Rank);

    /* Set the SQx bits for the selected rank */
    adc->SQR3 |= ADC_SQR3_RK(sConfig->Channel, sConfig->Rank);
  }
    /* For Rank 7 to 12 */
  else if (sConfig->Rank < 13)
  {
    /* Clear the old SQx bits for the selected rank */
    adc->SQR2 &= ~ADC_SQR2_RK(ADC_SQR2_SQ7, sConfig->Rank);

    /* Set the SQx bits for the selected rank */
    adc->SQR2 |= ADC_SQR2_RK(sConfig->Channel, sConfig->Rank);
  }
    /* For Rank 13 to 16 */
  else
  {
    /* Clear the old SQx bits for the selected rank */
    adc->SQR1 &= ~ADC_SQR1_RK(ADC_SQR1_SQ13, sConfig->Rank);

    /* Set the SQx bits for the selected rank */
    adc->SQR1 |= ADC_SQR1_RK(sConfig->Channel, sConfig->Rank);
  }

  /* if ADC1 Channel_18 is selected enable VBAT Channel */
  if ((adc == ADC1) && (sConfig->Channel == ADC_CHANNEL_VBAT))
  {
    /* Enable the VBAT channel*/
    ADC->CCR |= ADC_CCR_VBATE;
  }

  /* if ADC1 Channel_18 or Channel_17 is selected enable TSVREFE Channel(Temperature sensor and VREFINT) */
  if ((adc == ADC1) && ((sConfig->Channel == ADC_CHANNEL_TEMPSENSOR) || (sConfig->Channel == ADC_CHANNEL_VREFINT)))
  {
    /* Enable the TSVREFE channel*/
    ADC->CCR |= ADC_CCR_TSVREFE;

    if(sConfig->Channel == ADC_CHANNEL_TEMPSENSOR)
    {
      /* Delay for temperature sensor stabilization time */
      delay(ADC_TEMPSENSOR_DELAY_US);
    }
  }
}

void ADC_Start_DMA(ADC_TypeDef *adc, DMA_HandleTypeDef *hdma, unsigned short* pData, unsigned short* pData2, unsigned int Length)
{
  /* Enable the Peripheral */
  ADC_ENABLE(adc);
  delay(ADC_STAB_DELAY_US);

  /* Clear regular group conversion flag and overrun flag */
  /* (To ensure of no unknown state from potential previous ADC operations) */
  ADC_CLEAR_FLAG(adc, ADC_FLAG_EOC | ADC_FLAG_OVR);

  /* Enable ADC overrun interrupt */
  ADC_ENABLE_IT(adc, ADC_IT_OVR);

  /* Enable ADC DMA mode */
  adc->CR2 |= ADC_CR2_DMA;

    /* Start the DMA channel */
  DMA_Start_IT(hdma, (unsigned int)&adc->DR, (unsigned int)pData, (unsigned int)pData2, Length);

  /* Check if Multimode enabled */
  if(!(ADC->CCR & ADC_CCR_MULTI))
  {
    /* if no external trigger present enable software conversion of regular channels */
    if((adc->CR2 & ADC_CR2_EXTEN) == RESET)
    {
      /* Enable the selected ADC software conversion for regular group */
      adc->CR2 |= (unsigned int)ADC_CR2_SWSTART;
    }
  }
  else
  {
    /* if instance of handle correspond to ADC1 and  no external trigger present enable software conversion of regular channels */
    if((adc == ADC1) && ((adc->CR2 & ADC_CR2_EXTEN) == RESET))
    {
      /* Enable the selected ADC software conversion for regular group */
      adc->CR2 |= (unsigned int)ADC_CR2_SWSTART;
    }
    /* if dual mode is selected, ADC3 works independently. */
    /* check if the mode selected is not triple */
    if(!(ADC->CCR & ADC_CCR_MULTI_4) )
    {
      /* if instance of handle correspond to ADC3 and  no external trigger present enable software conversion of regular channels */
      if((adc == ADC3) && ((adc->CR2 & ADC_CR2_EXTEN) == RESET))
      {
        /* Enable the selected ADC software conversion for regular group */
        adc->CR2 |= (unsigned int)ADC_CR2_SWSTART;
      }
    }
  }
}
