#include <board.h>
#include <adc.h>

#if defined (STM32L100xC) || defined (STM32L151xC) || defined (STM32L152xC) || defined (STM32L162xC) || defined (STM32L151xCA) || defined (STM32L151xD) || defined (STM32L152xCA) || defined (STM32L152xD) || defined (STM32L162xCA) || defined (STM32L162xD) || defined (STM32L151xE) || defined (STM32L151xDX) || defined (STM32L152xE) || defined (STM32L152xDX) || defined (STM32L162xE) || defined (STM32L162xDX)
#define ADC_CR2_MASK_ADCINIT                                                 \
  (ADC_CR2_EXTEN | ADC_CR2_EXTSEL | ADC_CR2_ALIGN | ADC_CR2_EOCS | ADC_CR2_DDS | ADC_CR2_DELS | ADC_CR2_CFG | ADC_CR2_CONT)
#else
#define ADC_CR2_MASK_ADCINIT                                                 \
  (ADC_CR2_EXTEN | ADC_CR2_EXTSEL | ADC_CR2_ALIGN | ADC_CR2_EOCS | ADC_CR2_DDS | ADC_CR2_DELS | ADC_CR2_CONT)
#endif

#define ADC_SCAN_DISABLE         0U
#define ADC_SCAN_ENABLE          ADC_CR1_SCAN

#define ADC_CR2_DMACONTREQ(_DMACONTREQ_MODE_)                                  \
  ((_DMACONTREQ_MODE_) << POSITION_VAL(ADC_CR2_DDS))

#define ADC_CR2_CONTINUOUS(_CONTINUOUS_MODE_)                                  \
  ((_CONTINUOUS_MODE_) << POSITION_VAL(ADC_CR2_CONT))

#define ADC_CR1_SCAN_SET(_SCAN_MODE_)                                          \
  (( ((_SCAN_MODE_) == ADC_SCAN_ENABLE) || ((_SCAN_MODE_) == ENABLE)           \
   )? (ADC_SCAN_ENABLE) : (ADC_SCAN_DISABLE)                                   \
  )

#define ADC_CR1_DISCONTINUOUS_NUM(_NBR_DISCONTINUOUS_CONV_)                    \
  (((_NBR_DISCONTINUOUS_CONV_) - 1) << POSITION_VAL(ADC_CR1_DISCNUM))

#define ADC_SQR1_L_SHIFT(_NbrOfConversion_)                                    \
  (((_NbrOfConversion_) - 1) << POSITION_VAL(ADC_SQR1_L))

#define ADC_SQR5_RK(_CHANNELNB_, _RANKNB_)                                     \
  ((_CHANNELNB_) << (5 * ((_RANKNB_) - 1)))
#define ADC_SQR4_RK(_CHANNELNB_, _RANKNB_)                                     \
  ((_CHANNELNB_) << (5 * ((_RANKNB_) - 7)))
#define ADC_SQR3_RK(_CHANNELNB_, _RANKNB_)                                     \
  ((_CHANNELNB_) << (5 * ((_RANKNB_) - 13)))
#define ADC_SQR2_RK(_CHANNELNB_, _RANKNB_)                                     \
  ((_CHANNELNB_) << (5 * ((_RANKNB_) - 19)))
#define ADC_SQR1_RK(_CHANNELNB_, _RANKNB_)                                     \
  ((_CHANNELNB_) << (5 * ((_RANKNB_) - 25)))

#define ADC_SMPR3(_SAMPLETIME_, _CHANNELNB_)                                   \
  ((_SAMPLETIME_) << (3 * (_CHANNELNB_)))
#define ADC_SMPR2(_SAMPLETIME_, _CHANNELNB_)                                   \
  ((_SAMPLETIME_) << (3 * ((_CHANNELNB_) - 10)))

#if defined(STM32L151xCA) || defined (STM32L151xD) || defined (STM32L152xCA) || defined (STM32L152xD) || defined (STM32L162xCA) || defined (STM32L162xD) || defined(STM32L151xE) || defined(STM32L151xDX) || defined (STM32L152xE) || defined (STM32L152xDX) || defined (STM32L162xE) || defined (STM32L162xDX)
#define ADC_SMPR1_CHANNEL_MAX   ADC_CHANNEL_29
#else
#define ADC_SMPR1_CHANNEL_MAX   ADC_CHANNEL_26
#endif /* STM32L151xCA || STM32L151xD || STM32L152xCA || STM32L152xD || STM32L162xCA || STM32L162xD || STM32L151xE || STM32L151xDX || STM32L152xE || STM32L152xDX || STM32L162xE || STM32L162xDX */

#define ADC_SMPR1(_SAMPLETIME_, _CHANNELNB_)                                   \
  ((_SAMPLETIME_) << (3 * ((_CHANNELNB_) - 20)))

#if defined(STM32L151xCA) || defined (STM32L151xD) || defined (STM32L152xCA) || defined (STM32L152xD) || defined (STM32L162xCA) || defined (STM32L162xD) || defined(STM32L151xE) || defined(STM32L151xDX) || defined (STM32L152xE) || defined (STM32L152xDX) || defined (STM32L162xE) || defined (STM32L162xDX)
#define ADC_SMPR0(_SAMPLETIME_, _CHANNELNB_)                                   \
  ((_SAMPLETIME_) << (3 * ((_CHANNELNB_) - 30)))
#else
#define ADC_SMPR0(_SAMPLETIME_, _CHANNELNB_)                                   \
  (0x00000000U)
#endif /* STM32L151xCA || STM32L151xD || STM32L152xCA || STM32L152xD || STM32L162xCA || STM32L162xD || STM32L151xE || STM32L151xDX || STM32L152xE || STM32L152xDX || STM32L162xE || STM32L162xDX */

#if defined(STM32L151xCA) || defined (STM32L151xD) || defined (STM32L152xCA) || defined (STM32L152xD) || defined (STM32L162xCA) || defined (STM32L162xD) || defined(STM32L151xE) || defined(STM32L151xDX) || defined (STM32L152xE) || defined (STM32L152xDX) || defined (STM32L162xE) || defined (STM32L162xDX)
#define ADC_SMPR0_CHANNEL_SET(__INSTANCE__, _SAMPLETIME_, __CHANNEL__)         \
    MODIFY_REG((__INSTANCE__)->SMPR0,                                          \
               ADC_SMPR0(ADC_SMPR0_SMP30, (__CHANNEL__)),                      \
               ADC_SMPR0((_SAMPLETIME_), (__CHANNEL__))  )
#else
#define ADC_SMPR0_CHANNEL_SET(__HANDLE__, _SAMPLETIME_, __CHANNEL__)
#endif /* STM32L151xCA || STM32L151xD || STM32L152xCA || STM32L152xD || STM32L162xCA || STM32L162xD || STM32L151xE || STM32L151xDX || STM32L152xE || STM32L152xDX || STM32L162xE || STM32L162xDX */

unsigned int ADC_Init(ADC_TypeDef *instance, ADC_InitTypeDef *init)
{
  unsigned int tmp_cr1, tmp_cr2;

  /* Configuration of common ADC clock: clock source HSI with selectable    */
  /* prescaler                                                              */
  MODIFY_REG(ADC->CCR                 ,
             ADC_CCR_ADCPRE           ,
             init->ClockPrescaler);

  /* Configuration of ADC:                                                  */
  /*  - external trigger polarity                                           */
  /*  - End of conversion selection                                         */
  /*  - DMA continuous request                                              */
  /*  - Channels bank (Banks availability depends on devices categories)    */
  /*  - continuous conversion mode                                          */
  tmp_cr2 = init->DataAlign                                  |
            init->EOCSelection                               |
            ADC_CR2_DMACONTREQ(init->DMAContinuousRequests) |
            init->ChannelsBank                               |
            ADC_CR2_CONTINUOUS(init->ContinuousConvMode);

  /* Enable external trigger if trigger selection is different of software  */
  /* start.                                                                 */
  /* Note: This configuration keeps the hardware feature of parameter       */
  /*       ExternalTrigConvEdge "trigger edge none" equivalent to           */
  /*       software start.                                                  */
  if (init->ExternalTrigConv != ADC_SOFTWARE_START)
  {
    tmp_cr2 |= init->ExternalTrigConv |init->ExternalTrigConvEdge;
  }

  tmp_cr2 |= init->LowPowerAutoWait;

  tmp_cr1 = init->Resolution                     |
            init->LowPowerAutoPowerOff           |
            ADC_CR1_SCAN_SET(init->ScanConvMode);

  /* Enable discontinuous mode only if continuous mode is disabled */
  /* Note: If parameter "Init.ScanConvMode" is set to disable, parameter  */
  /*       discontinuous is set anyway, but has no effect on ADC HW.      */
  if (init->DiscontinuousConvMode == ENABLE)
  {
    if (init->ContinuousConvMode == DISABLE)
    {
      /* Enable the selected ADC regular discontinuous mode */
      /* Set the number of channels to be converted in discontinuous mode */
      SET_BIT(tmp_cr1, ADC_CR1_DISCEN                                            |
                       ADC_CR1_DISCONTINUOUS_NUM(init->NbrOfDiscConversion));
    }
    else
      return 0;
  }

  /* Update ADC configuration register CR1 with previous settings */
  MODIFY_REG(instance->CR1,
             ADC_CR1_RES     |
             ADC_CR1_PDI     |
             ADC_CR1_PDD     |
             ADC_CR1_DISCNUM |
             ADC_CR1_DISCEN  |
             ADC_CR1_SCAN     ,
             tmp_cr1);

  /* Update ADC configuration register CR2 with previous settings */
  MODIFY_REG(instance->CR2,
             ADC_CR2_MASK_ADCINIT,
             tmp_cr2);

  /* Configuration of regular group sequencer:                              */
  /* - if scan mode is disabled, regular channels sequence length is set to */
  /*   0x00: 1 channel converted (channel on regular rank 1)                */
  /*   Parameter "NbrOfConversion" is discarded.                            */
  /*   Note: Scan mode is present by hardware on this device and, if        */
  /*   disabled, discards automatically nb of conversions. Anyway, nb of    */
  /*   conversions is forced to 0x00 for alignment over all STM32 devices.  */
  /* - if scan mode is enabled, regular channels sequence length is set to  */
  /*   parameter "NbrOfConversion"                                          */
  if (ADC_CR1_SCAN_SET(init->ScanConvMode) == ADC_SCAN_ENABLE)
  {
    MODIFY_REG(instance->SQR1                         ,
               ADC_SQR1_L                                   ,
               ADC_SQR1_L_SHIFT(init->NbrOfConversion));
  }
  else
  {
    MODIFY_REG(instance->SQR1,
               ADC_SQR1_L,
               0);
  }

  return 1;
}

void ADC_StartConversion(ADC_TypeDef *instance)
{
  instance->CR2 |= ADC_CR2_SWSTART;
}

void ADC_Enable(ADC_TypeDef *instance)
{
  instance->CR2 |= ADC_CR2_ADON;
}

void ADC_ConfigChannel(ADC_TypeDef* instance, ADC_ChannelConfTypeDef* sConfig)
{
  /* Regular sequence configuration */
  /* For Rank 1 to 6 */
  if (sConfig->Rank < 7)
  {
    MODIFY_REG(instance->SQR5,
               ADC_SQR5_RK(ADC_SQR5_SQ1, sConfig->Rank),
               ADC_SQR5_RK(sConfig->Channel, sConfig->Rank) );
  }
    /* For Rank 7 to 12 */
  else if (sConfig->Rank < 13)
  {
    MODIFY_REG(instance->SQR4,
               ADC_SQR4_RK(ADC_SQR4_SQ7, sConfig->Rank),
               ADC_SQR4_RK(sConfig->Channel, sConfig->Rank) );
  }
    /* For Rank 13 to 18 */
  else if (sConfig->Rank < 19)
  {
    MODIFY_REG(instance->SQR3,
               ADC_SQR3_RK(ADC_SQR3_SQ13, sConfig->Rank),
               ADC_SQR3_RK(sConfig->Channel, sConfig->Rank) );
  }
    /* For Rank 19 to 24 */
  else if (sConfig->Rank < 25)
  {
    MODIFY_REG(instance->SQR2,
               ADC_SQR2_RK(ADC_SQR2_SQ19, sConfig->Rank),
               ADC_SQR2_RK(sConfig->Channel, sConfig->Rank) );
  }
    /* For Rank 25 to 28 */
  else
  {
    MODIFY_REG(instance->SQR1,
               ADC_SQR1_RK(ADC_SQR1_SQ25, sConfig->Rank),
               ADC_SQR1_RK(sConfig->Channel, sConfig->Rank) );
  }


  /* Channel sampling time configuration */
  /* For channels 0 to 9 */
  if (sConfig->Channel < ADC_CHANNEL_10)
  {
    MODIFY_REG(instance->SMPR3,
               ADC_SMPR3(ADC_SMPR3_SMP0, sConfig->Channel),
               ADC_SMPR3(sConfig->SamplingTime, sConfig->Channel) );
  }
    /* For channels 10 to 19 */
  else if (sConfig->Channel < ADC_CHANNEL_20)
  {
    MODIFY_REG(instance->SMPR2,
               ADC_SMPR2(ADC_SMPR2_SMP10, sConfig->Channel),
               ADC_SMPR2(sConfig->SamplingTime, sConfig->Channel) );
  }
    /* For channels 20 to 26 for devices Cat.1, Cat.2, Cat.3 */
    /* For channels 20 to 29 for devices Cat4, Cat.5 */
  else if (sConfig->Channel <= ADC_SMPR1_CHANNEL_MAX)
  {
    MODIFY_REG(instance->SMPR1,
               ADC_SMPR1(ADC_SMPR1_SMP20, sConfig->Channel),
               ADC_SMPR1(sConfig->SamplingTime, sConfig->Channel) );
  }
    /* For channels 30 to 31 for devices Cat4, Cat.5 */
  else
  {
    ADC_SMPR0_CHANNEL_SET(instance, sConfig->SamplingTime, sConfig->Channel);
  }

  /* If ADC1 Channel_16 or Channel_17 is selected, enable Temperature sensor  */
  /* and VREFINT measurement path.                                            */
  if ((sConfig->Channel == ADC_CHANNEL_TEMPSENSOR) ||
      (sConfig->Channel == ADC_CHANNEL_VREFINT))
  {
    if (READ_BIT(ADC->CCR, ADC_CCR_TSVREFE) == RESET)
    {
      SET_BIT(ADC->CCR, ADC_CCR_TSVREFE);

      if (sConfig->Channel == ADC_CHANNEL_TEMPSENSOR)
        delay(10);
    }
  }
}

unsigned int ADC_GetValue(ADC_TypeDef* instance)
{
  return instance->DR;
}

unsigned int ADC_PollForConversion(ADC_TypeDef* instance, unsigned int timeout)
{
  while(--timeout && !(instance->SR & ADC_SR_EOC))
    ;

  return timeout;
}
