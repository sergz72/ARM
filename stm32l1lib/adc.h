#ifndef _ADC_H
#define _ADC_H

typedef struct
{
  unsigned int ClockPrescaler;        /*!< Select ADC clock source (asynchronous clock derived from HSI RC oscillator) and clock prescaler.
                                       This parameter can be a value of @ref ADC_ClockPrescaler
                                       Note: In case of usage of channels on injected group, ADC frequency should be lower than AHB clock frequency /4 for resolution 12 or 10 bits, 
                                             AHB clock frequency /3 for resolution 8 bits, AHB clock frequency /2 for resolution 6 bits.
                                       Note: HSI RC oscillator must be preliminarily enabled at RCC top level. */
  unsigned int Resolution;            /*!< Configures the ADC resolution. 
                                       This parameter can be a value of @ref ADC_Resolution */
  unsigned int DataAlign;             /*!< Specifies ADC data alignment to right (MSB on register bit 11 and LSB on register bit 0) (default setting)
                                       or to left (if regular group: MSB on register bit 15 and LSB on register bit 4, if injected group (MSB kept as signed value due to potential negative value after offset application): MSB on register bit 14 and LSB on register bit 3).
                                       This parameter can be a value of @ref ADC_Data_align */
  unsigned int ScanConvMode;          /*!< Configures the sequencer of regular and injected groups.
                                       This parameter can be associated to parameter 'DiscontinuousConvMode' to have main sequence subdivided in successive parts.
                                       If disabled: Conversion is performed in single mode (one channel converted, the one defined in rank 1).
                                                    Parameters 'NbrOfConversion' and 'InjectedNbrOfConversion' are discarded (equivalent to set to 1).
                                       If enabled:  Conversions are performed in sequence mode (multiple ranks defined by 'NbrOfConversion'/'InjectedNbrOfConversion' and each channel rank).
                                                    Scan direction is upward: from rank1 to rank 'n'.
                                       This parameter can be a value of @ref ADC_Scan_mode */
  unsigned int EOCSelection;          /*!< Specifies what EOC (End Of Conversion) flag is used for conversion by polling and interruption: end of conversion of each rank or complete sequence.
                                       This parameter can be a value of @ref ADC_EOCSelection.
                                       Note: For injected group, end of conversion (flag&IT) is raised only at the end of the sequence.
                                             Therefore, if end of conversion is set to end of each conversion, injected group should not be used with interruption (HAL_ADCEx_InjectedStart_IT)
                                             or polling (HAL_ADCEx_InjectedStart and HAL_ADCEx_InjectedPollForConversion). By the way, polling is still possible since driver will use an estimated timing for end of injected conversion.
                                       Note: If overrun feature is intended to be used, use ADC in mode 'interruption' (function HAL_ADC_Start_IT() ) with parameter EOCSelection set to end of each conversion or in mode 'transfer by DMA' (function HAL_ADC_Start_DMA()).
                                             If overrun feature is intended to be bypassed, use ADC in mode 'polling' or 'interruption' with parameter EOCSelection must be set to end of sequence */
  unsigned int LowPowerAutoWait;      /*!< Selects the dynamic low power Auto Delay: new conversion start only when the previous
                                       conversion (for regular group) or previous sequence (for injected group) has been treated by user software, using function HAL_ADC_GetValue() or HAL_ADCEx_InjectedGetValue().
                                       This feature automatically adapts the speed of ADC to the speed of the system that reads the data. Moreover, this avoids risk of overrun for low frequency applications. 
                                       This parameter can be a value of @ref ADC_LowPowerAutoWait.
                                       Note: Do not use with interruption or DMA (HAL_ADC_Start_IT(), HAL_ADC_Start_DMA()) since they have to clear immediately the EOC flag to free the IRQ vector sequencer.
                                             Do use with polling: 1. Start conversion with HAL_ADC_Start(), 2. Later on, when conversion data is needed: use HAL_ADC_PollForConversion() to ensure that conversion is completed
                                             and use HAL_ADC_GetValue() to retrieve conversion result and trig another conversion (in case of usage of injected group, use the equivalent functions HAL_ADCExInjected_Start(), HAL_ADCEx_InjectedGetValue(), ...).
                                       Note: ADC clock latency and some timing constraints depending on clock prescaler have to be taken into account: refer to reference manual (register ADC_CR2 bit DELS description). */
  unsigned int LowPowerAutoPowerOff;  /*!< Selects the auto-off mode: the ADC automatically powers-off after a conversion and automatically wakes-up when a new conversion is triggered (with startup time between trigger and start of sampling).
                                       This feature can be combined with automatic wait mode (parameter 'LowPowerAutoWait').
                                       This parameter can be a value of @ref ADC_LowPowerAutoPowerOff. */
  unsigned int ChannelsBank;          /*!< Selects the ADC channels bank.
                                       This parameter can be a value of @ref ADC_ChannelsBank.
                                       Note: Banks availability depends on devices categories.
                                       Note: To change bank selection on the fly, without going through execution of 'HAL_ADC_Init()', macro '__HAL_ADC_CHANNELS_BANK()' can be used directly. */
  FunctionalState ContinuousConvMode; /*!< Specifies whether the conversion is performed in single mode (one conversion) or continuous mode for regular group,
                                       after the selected trigger occurred (software start or external trigger).
                                       This parameter can be set to ENABLE or DISABLE. */
#if defined(STM32L100xC) || defined (STM32L151xC) || defined (STM32L152xC) || defined (STM32L162xC) || defined(STM32L151xCA) || defined (STM32L151xD) || defined (STM32L152xCA) || defined (STM32L152xD) || defined (STM32L162xCA) || defined (STM32L162xD) || defined(STM32L151xE) || defined(STM32L151xDX) || defined (STM32L152xE) || defined (STM32L152xDX) || defined (STM32L162xE) || defined (STM32L162xDX)
  unsigned int NbrOfConversion;       /*!< Specifies the number of ranks that will be converted within the regular group sequencer.
                                       To use regular group sequencer and convert several ranks, parameter 'ScanConvMode' must be enabled.
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 28. */
#else
  unsigned int NbrOfConversion;       /*!< Specifies the number of ranks that will be converted within the regular group sequencer.
                                       To use regular group sequencer and convert several ranks, parameter 'ScanConvMode' must be enabled.
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 27. */
#endif /* STM32L100xC || STM32L151xC || STM32L152xC || STM32L162xC || STM32L151xCA || STM32L151xD || STM32L152xCA || STM32L152xD || STM32L162xCA || STM32L162xD || STM32L151xE || STM32L151xDX || STM32L152xE || STM32L152xDX || STM32L162xE || STM32L162xDX */
  FunctionalState DiscontinuousConvMode; /*!< Specifies whether the conversions sequence of regular group is performed in Complete-sequence/Discontinuous-sequence (main sequence subdivided in successive parts).
                                       Discontinuous mode is used only if sequencer is enabled (parameter 'ScanConvMode'). If sequencer is disabled, this parameter is discarded.
                                       Discontinuous mode can be enabled only if continuous mode is disabled. If continuous mode is enabled, this parameter setting is discarded.
                                       This parameter can be set to ENABLE or DISABLE. */
  unsigned int NbrOfDiscConversion;   /*!< Specifies the number of discontinuous conversions in which the  main sequence of regular group (parameter NbrOfConversion) will be subdivided.
                                       If parameter 'DiscontinuousConvMode' is disabled, this parameter is discarded.
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 8. */
  unsigned int ExternalTrigConv;      /*!< Selects the external event used to trigger the conversion start of regular group.
                                       If set to ADC_SOFTWARE_START, external triggers are disabled.
                                       If set to external trigger source, triggering is on event rising edge by default.
                                       This parameter can be a value of @ref ADC_External_trigger_source_Regular */
  unsigned int ExternalTrigConvEdge;  /*!< Selects the external trigger edge of regular group.
                                       If trigger is set to ADC_SOFTWARE_START, this parameter is discarded.
                                       This parameter can be a value of @ref ADC_External_trigger_edge_Regular */
  FunctionalState DMAContinuousRequests; /*!< Specifies whether the DMA requests are performed in one shot mode (DMA transfer stop when number of conversions is reached)
                                       or in Continuous mode (DMA transfer unlimited, whatever number of conversions).
                                       Note: In continuous mode, DMA must be configured in circular mode. Otherwise an overrun will be triggered when DMA buffer maximum pointer is reached.
                                       Note: This parameter must be modified when no conversion is on going on both regular and injected groups (ADC disabled, or ADC enabled without continuous mode or external trigger that could launch a conversion).
                                       This parameter can be set to ENABLE or DISABLE. */
} ADC_InitTypeDef;

typedef struct
{
  unsigned int Channel;                /*!< Specifies the channel to configure into ADC regular group.
                                        This parameter can be a value of @ref ADC_channels
                                        Note: Depending on devices, some channels may not be available on package pins. Refer to device datasheet for channels availability.
                                              Maximum number of channels by device category (without taking in account each device package constraints): 
                                              STM32L1 category 1, 2: 24 channels on external pins + 3 channels on internal measurement paths (VrefInt, Temp sensor, Vcomp): Channel 0 to channel 26.
                                              STM32L1 category 3:    25 channels on external pins + 3 channels on internal measurement paths (VrefInt, Temp sensor, Vcomp): Channel 0 to channel 26, 1 additional channel in bank B. Note: OPAMP1 and OPAMP2 are connected internally but not increasing internal channels number: they are sharing ADC input with external channels ADC_IN3 and ADC_IN8.
                                              STM32L1 category 4, 5: 40 channels on external pins + 3 channels on internal measurement paths (VrefInt, Temp sensor, Vcomp): Channel 0 to channel 31, 11 additional channels in bank B. Note: OPAMP1 and OPAMP2 are connected internally but not increasing internal channels number: they are sharing ADC input with external channels ADC_IN3 and ADC_IN8.
                                        Note: In case of peripherals OPAMPx not used: 3 channels (3, 8, 13) can be configured as direct channels (fast channels). Refer to macro ' __HAL_ADC_CHANNEL_SPEED_FAST() '.
                                        Note: In case of peripheral OPAMP3 and ADC channel OPAMP3 used (OPAMP3 available on STM32L1 devices Cat.4 only): the analog switch COMP1_SW1 must be closed. Refer to macro: ' __HAL_OPAMP_OPAMP3OUT_CONNECT_ADC_COMP1() '. */
  unsigned int Rank;                   /*!< Specifies the rank in the regular group sequencer.
                                        This parameter can be a value of @ref ADC_regular_rank
                                        Note: In case of need to disable a channel or change order of conversion sequencer, rank containing a previous channel setting can be overwritten by the new channel setting (or parameter number of conversions can be adjusted) */
  unsigned int SamplingTime;           /*!< Sampling time value to be set for the selected channel.
                                        Unit: ADC clock cycles
                                        Conversion time is the addition of sampling time and processing time (12 ADC clock cycles at ADC resolution 12 bits, 11 cycles at 10 bits, 9 cycles at 8 bits, 7 cycles at 6 bits).
                                        This parameter can be a value of @ref ADC_sampling_times
                                        Caution: This parameter updates the parameter property of the channel, that can be used into regular and/or injected groups.
                                                 If this same channel has been previously configured in the other group (regular/injected), it will be updated to last setting.
                                        Note: In case of usage of internal measurement channels (VrefInt/Vbat/TempSensor),
                                              sampling time constraints must be respected (sampling time can be adjusted in function of ADC clock frequency and sampling time setting)
                                              Refer to device datasheet for timings values, parameters TS_vrefint, TS_temp (values rough order: 4us min). */
} ADC_ChannelConfTypeDef;

/* External triggers of regular group for ADC1 */
#define ADC_EXTERNALTRIGCONV_T2_CC3      ADC_EXTERNALTRIG_T2_CC3
#define ADC_EXTERNALTRIGCONV_T2_CC2      ADC_EXTERNALTRIG_T2_CC2
#define ADC_EXTERNALTRIGCONV_T2_TRGO     ADC_EXTERNALTRIG_T2_TRGO
#define ADC_EXTERNALTRIGCONV_T3_CC1      ADC_EXTERNALTRIG_T3_CC1
#define ADC_EXTERNALTRIGCONV_T3_CC3      ADC_EXTERNALTRIG_T3_CC3
#define ADC_EXTERNALTRIGCONV_T3_TRGO     ADC_EXTERNALTRIG_T3_TRGO
#define ADC_EXTERNALTRIGCONV_T4_CC4      ADC_EXTERNALTRIG_T4_CC4
#define ADC_EXTERNALTRIGCONV_T4_TRGO     ADC_EXTERNALTRIG_T4_TRGO
#define ADC_EXTERNALTRIGCONV_T6_TRGO     ADC_EXTERNALTRIG_T6_TRGO
#define ADC_EXTERNALTRIGCONV_T9_CC2      ADC_EXTERNALTRIG_T9_CC2
#define ADC_EXTERNALTRIGCONV_T9_TRGO     ADC_EXTERNALTRIG_T9_TRGO
#define ADC_EXTERNALTRIGCONV_EXT_IT11    ADC_EXTERNALTRIG_EXT_IT11
#define ADC_SOFTWARE_START               0x00000010U

#define ADC_CHANNEL_0           (0x00000000U)                                                                                     /* Channel different in bank A and bank B */
#define ADC_CHANNEL_1           ((unsigned int)(                                                                    ADC_SQR5_SQ1_0))  /* Channel different in bank A and bank B */
#define ADC_CHANNEL_2           ((unsigned int)(                                                   ADC_SQR5_SQ1_1                 ))  /* Channel different in bank A and bank B */
#define ADC_CHANNEL_3           ((unsigned int)(                                                   ADC_SQR5_SQ1_1 | ADC_SQR5_SQ1_0))  /* Channel different in bank A and bank B */
#define ADC_CHANNEL_4           ((unsigned int)(                                  ADC_SQR5_SQ1_2                                  ))  /* Direct (fast) channel */
#define ADC_CHANNEL_5           ((unsigned int)(                                  ADC_SQR5_SQ1_2                  | ADC_SQR5_SQ1_0))  /* Direct (fast) channel */
#define ADC_CHANNEL_6           ((unsigned int)(                                  ADC_SQR5_SQ1_2 | ADC_SQR5_SQ1_1                 ))  /* Channel different in bank A and bank B */
#define ADC_CHANNEL_7           ((unsigned int)(                                  ADC_SQR5_SQ1_2 | ADC_SQR5_SQ1_1 | ADC_SQR5_SQ1_0))  /* Channel different in bank A and bank B */
#define ADC_CHANNEL_8           ((unsigned int)(                 ADC_SQR5_SQ1_3                                                   ))  /* Channel different in bank A and bank B */
#define ADC_CHANNEL_9           ((unsigned int)(                 ADC_SQR5_SQ1_3                                   | ADC_SQR5_SQ1_0))  /* Channel different in bank A and bank B */
#define ADC_CHANNEL_10          ((unsigned int)(                 ADC_SQR5_SQ1_3                  | ADC_SQR5_SQ1_1                 ))  /* Channel different in bank A and bank B */
#define ADC_CHANNEL_11          ((unsigned int)(                 ADC_SQR5_SQ1_3                  | ADC_SQR5_SQ1_1 | ADC_SQR5_SQ1_0))  /* Channel different in bank A and bank B */
#define ADC_CHANNEL_12          ((unsigned int)(                 ADC_SQR5_SQ1_3 | ADC_SQR5_SQ1_2                                  ))  /* Channel different in bank A and bank B */
#define ADC_CHANNEL_13          ((unsigned int)(                 ADC_SQR5_SQ1_3 | ADC_SQR5_SQ1_2                  | ADC_SQR5_SQ1_0))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_14          ((unsigned int)(                 ADC_SQR5_SQ1_3 | ADC_SQR5_SQ1_2 | ADC_SQR5_SQ1_1                 ))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_15          ((unsigned int)(                 ADC_SQR5_SQ1_3 | ADC_SQR5_SQ1_2 | ADC_SQR5_SQ1_1 | ADC_SQR5_SQ1_0))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_16          ((unsigned int)(ADC_SQR5_SQ1_4                                                                    ))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_17          ((unsigned int)(ADC_SQR5_SQ1_4                                                    | ADC_SQR5_SQ1_0))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_18          ((unsigned int)(ADC_SQR5_SQ1_4                                   | ADC_SQR5_SQ1_1                 ))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_19          ((unsigned int)(ADC_SQR5_SQ1_4                                   | ADC_SQR5_SQ1_1 | ADC_SQR5_SQ1_0))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_20          ((unsigned int)(ADC_SQR5_SQ1_4                  | ADC_SQR5_SQ1_2                                  ))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_21          ((unsigned int)(ADC_SQR5_SQ1_4                  | ADC_SQR5_SQ1_2                  | ADC_SQR5_SQ1_0))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_22          ((unsigned int)(ADC_SQR5_SQ1_4                  | ADC_SQR5_SQ1_2 | ADC_SQR5_SQ1_1                 ))  /* Direct (fast) channel */
#define ADC_CHANNEL_23          ((unsigned int)(ADC_SQR5_SQ1_4                  | ADC_SQR5_SQ1_2 | ADC_SQR5_SQ1_1 | ADC_SQR5_SQ1_0))  /* Direct (fast) channel */
#define ADC_CHANNEL_24          ((unsigned int)(ADC_SQR5_SQ1_4 | ADC_SQR5_SQ1_3                                                   ))  /* Direct (fast) channel */
#define ADC_CHANNEL_25          ((unsigned int)(ADC_SQR5_SQ1_4 | ADC_SQR5_SQ1_3                                   | ADC_SQR5_SQ1_0))  /* Direct (fast) channel */
#define ADC_CHANNEL_26          ((unsigned int)(ADC_SQR5_SQ1_4 | ADC_SQR5_SQ1_3                  | ADC_SQR5_SQ1_1                 ))  /* Channel common to both bank A and bank B */
#if defined(STM32L151xCA) || defined (STM32L151xD) || defined (STM32L152xCA) || defined (STM32L152xD) || defined (STM32L162xCA) || defined (STM32L162xD) || defined(STM32L151xE) || defined(STM32L151xDX) || defined (STM32L152xE) || defined (STM32L152xDX) || defined (STM32L162xE) || defined (STM32L162xDX)
#define ADC_CHANNEL_27          ((unsigned int)(ADC_SQR5_SQ1_4 | ADC_SQR5_SQ1_3                  | ADC_SQR5_SQ1_1 | ADC_SQR5_SQ1_0))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_28          ((unsigned int)(ADC_SQR5_SQ1_4 | ADC_SQR5_SQ1_3 | ADC_SQR5_SQ1_2                                  ))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_29          ((unsigned int)(ADC_SQR5_SQ1_4 | ADC_SQR5_SQ1_3 | ADC_SQR5_SQ1_2                  | ADC_SQR5_SQ1_0))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_30          ((unsigned int)(ADC_SQR5_SQ1_4 | ADC_SQR5_SQ1_3 | ADC_SQR5_SQ1_2 | ADC_SQR5_SQ1_1                 ))  /* Channel common to both bank A and bank B */
#define ADC_CHANNEL_31          ((unsigned int)(ADC_SQR5_SQ1_4 | ADC_SQR5_SQ1_3 | ADC_SQR5_SQ1_2 | ADC_SQR5_SQ1_1 | ADC_SQR5_SQ1_0))  /* Channel common to both bank A and bank B */
#endif /* STM32L151xCA || STM32L151xD || STM32L152xCA || STM32L152xD || STM32L162xCA || STM32L162xD || STM32L151xE || STM32L151xDX || STM32L152xE || STM32L152xDX || STM32L162xE || STM32L162xDX */

#define ADC_CHANNEL_TEMPSENSOR  ADC_CHANNEL_16  /* ADC internal channel (no connection on device pin). Channel common to both bank A and bank B. */
#define ADC_CHANNEL_VREFINT     ADC_CHANNEL_17  /* ADC internal channel (no connection on device pin). Channel common to both bank A and bank B. */
#define ADC_CHANNEL_VCOMP       ADC_CHANNEL_26  /* ADC internal channel (no connection on device pin). Channel common to both bank A and bank B. */

#if defined(STM32L100xC) || defined (STM32L151xC) || defined (STM32L152xC) || defined (STM32L162xC) || defined(STM32L151xCA) || defined (STM32L151xD) || defined (STM32L152xCA) || defined (STM32L152xD) || defined (STM32L162xCA) || defined (STM32L162xD) || defined(STM32L151xE) || defined(STM32L151xDX) || defined (STM32L152xE) || defined (STM32L152xDX) || defined (STM32L162xE) || defined (STM32L162xDX)
#define ADC_CHANNEL_VOPAMP1     ADC_CHANNEL_3   /* Internal connection from OPAMP1 output to ADC switch matrix */
#define ADC_CHANNEL_VOPAMP2     ADC_CHANNEL_8   /* Internal connection from OPAMP2 output to ADC switch matrix */
#if defined(STM32L151xCA) || defined (STM32L151xD) || defined (STM32L152xCA) || defined (STM32L152xD) || defined (STM32L162xCA) || defined (STM32L162xD)
#define ADC_CHANNEL_VOPAMP3     ADC_CHANNEL_13  /* Internal connection from OPAMP3 output to ADC switch matrix */
#endif /* STM32L151xCA || STM32L151xD || STM32L152xCA || STM32L152xD || STM32L162xCA || STM32L162xD */
#endif /* STM32L100xC || STM32L151xC || STM32L152xC || STM32L162xC || STM32L151xCA || STM32L151xD || STM32L152xCA || STM32L152xD || STM32L162xCA || STM32L162xD || STM32L151xE || STM32L151xDX || STM32L152xE || STM32L152xDX || STM32L162xE || STM32L162xDX */

#define ADC_SAMPLETIME_4CYCLES      (0x00000000U)                                     /*!< Sampling time 4 ADC clock cycles */
#define ADC_SAMPLETIME_9CYCLES      ((unsigned int) ADC_SMPR3_SMP0_0)                     /*!< Sampling time 9 ADC clock cycles */
#define ADC_SAMPLETIME_16CYCLES     ((unsigned int) ADC_SMPR3_SMP0_1)                     /*!< Sampling time 16 ADC clock cycles */
#define ADC_SAMPLETIME_24CYCLES     ((unsigned int)(ADC_SMPR3_SMP0_1 | ADC_SMPR3_SMP0_0)) /*!< Sampling time 24 ADC clock cycles */
#define ADC_SAMPLETIME_48CYCLES     ((unsigned int) ADC_SMPR3_SMP0_2)                     /*!< Sampling time 48 ADC clock cycles */
#define ADC_SAMPLETIME_96CYCLES     ((unsigned int)(ADC_SMPR3_SMP0_2 | ADC_SMPR3_SMP0_0)) /*!< Sampling time 96 ADC clock cycles */
#define ADC_SAMPLETIME_192CYCLES    ((unsigned int)(ADC_SMPR3_SMP0_2 | ADC_SMPR3_SMP0_1)) /*!< Sampling time 192 ADC clock cycles */
#define ADC_SAMPLETIME_384CYCLES    ((unsigned int) ADC_SMPR3_SMP0)                       /*!< Sampling time 384 ADC clock cycles */

#define ADC_CLOCK_ASYNC_DIV1          0U                  /*!< ADC asynchronous clock derived from ADC dedicated HSI without prescaler */
#define ADC_CLOCK_ASYNC_DIV2          ADC_CCR_ADCPRE_0    /*!< ADC asynchronous clock derived from ADC dedicated HSI divided by a prescaler of 2 */
#define ADC_CLOCK_ASYNC_DIV4          ADC_CCR_ADCPRE_1    /*!< ADC asynchronous clock derived from ADC dedicated HSI divided by a prescaler of 4 */

#define ADC_RESOLUTION_12B      0U                  /*!<  ADC 12-bit resolution */
#define ADC_RESOLUTION_10B      ADC_CR1_RES_0       /*!<  ADC 10-bit resolution */
#define ADC_RESOLUTION_8B       ADC_CR1_RES_1       /*!<  ADC 8-bit resolution */
#define ADC_RESOLUTION_6B       ADC_CR1_RES         /*!<  ADC 6-bit resolution */

#define ADC_DATAALIGN_RIGHT      0U
#define ADC_DATAALIGN_LEFT       ADC_CR2_ALIGN

#define ADC_SCAN_DISABLE         0U
#define ADC_SCAN_ENABLE          ADC_CR1_SCAN

#define ADC_EXTERNALTRIGCONVEDGE_NONE           0U
#define ADC_EXTERNALTRIGCONVEDGE_RISING         ADC_CR2_EXTEN_0
#define ADC_EXTERNALTRIGCONVEDGE_FALLING        ADC_CR2_EXTEN_1
#define ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING  ADC_CR2_EXTEN

#define ADC_EOC_SEQ_CONV            0U
#define ADC_EOC_SINGLE_CONV         ADC_CR2_EOCS

#define ADC_AUTOWAIT_DISABLE                0U
#define ADC_AUTOWAIT_UNTIL_DATA_READ        (                                  ADC_CR2_DELS_0) /*!< Insert a delay between ADC conversions: infinite delay, until the result of previous conversion is read */
#define ADC_AUTOWAIT_7_APBCLOCKCYCLES       (                 ADC_CR2_DELS_1                 ) /*!< Insert a delay between ADC conversions: 7 APB clock cycles */
#define ADC_AUTOWAIT_15_APBCLOCKCYCLES      (                 ADC_CR2_DELS_1 | ADC_CR2_DELS_0) /*!< Insert a delay between ADC conversions: 15 APB clock cycles */
#define ADC_AUTOWAIT_31_APBCLOCKCYCLES      (ADC_CR2_DELS_2                                  ) /*!< Insert a delay between ADC conversions: 31 APB clock cycles */
#define ADC_AUTOWAIT_63_APBCLOCKCYCLES      (ADC_CR2_DELS_2                  | ADC_CR2_DELS_0) /*!< Insert a delay between ADC conversions: 63 APB clock cycles */
#define ADC_AUTOWAIT_127_APBCLOCKCYCLES     (ADC_CR2_DELS_2 | ADC_CR2_DELS_1                 ) /*!< Insert a delay between ADC conversions: 127 APB clock cycles */
#define ADC_AUTOWAIT_255_APBCLOCKCYCLES     (ADC_CR2_DELS_2 | ADC_CR2_DELS_1 | ADC_CR2_DELS_0) /*!< Insert a delay between ADC conversions: 255 APB clock cycles */

#define ADC_AUTOPOWEROFF_DISABLE            0U
#define ADC_AUTOPOWEROFF_IDLE_PHASE         ADC_CR1_PDI                     /*!< ADC power off when ADC is not converting (idle phase) */
#define ADC_AUTOPOWEROFF_DELAY_PHASE        ADC_CR1_PDD                     /*!< ADC power off when a delay is inserted between conversions (see parameter ADC_LowPowerAutoWait) */
#define ADC_AUTOPOWEROFF_IDLE_DELAY_PHASES  (ADC_CR1_PDI | ADC_CR1_PDD)     /*!< ADC power off when ADC is not converting (idle phase) and when a delay is inserted between conversions */

#if defined(STM32L100xC) || defined (STM32L151xC) || defined (STM32L152xC) || defined (STM32L162xC) || defined(STM32L151xCA) || defined (STM32L151xD) || defined (STM32L152xCA) || defined (STM32L152xD) || defined (STM32L162xCA) || defined (STM32L162xD) || defined(STM32L151xE) || defined(STM32L151xDX) || defined (STM32L152xE) || defined (STM32L152xDX) || defined (STM32L162xE) || defined (STM32L162xDX)
#define ADC_CHANNELS_BANK_A                 0U
#define ADC_CHANNELS_BANK_B                 ADC_CR2_CFG
#else
#define ADC_CHANNELS_BANK_A                 0U
#endif /* STM32L100xC || STM32L151xC || STM32L152xC || STM32L162xC || STM32L151xCA || STM32L151xD || STM32L152xCA || STM32L152xD || STM32L162xCA || STM32L162xD || STM32L151xE || STM32L151xDX || STM32L152xE || STM32L152xDX || STM32L162xE || STM32L162xDX */

#define ADC_REGULAR_RANK_1    (0x00000001U)
#define ADC_REGULAR_RANK_2    (0x00000002U)
#define ADC_REGULAR_RANK_3    (0x00000003U)
#define ADC_REGULAR_RANK_4    (0x00000004U)
#define ADC_REGULAR_RANK_5    (0x00000005U)
#define ADC_REGULAR_RANK_6    (0x00000006U)
#define ADC_REGULAR_RANK_7    (0x00000007U)
#define ADC_REGULAR_RANK_8    (0x00000008U)
#define ADC_REGULAR_RANK_9    (0x00000009U)
#define ADC_REGULAR_RANK_10   (0x0000000AU)
#define ADC_REGULAR_RANK_11   (0x0000000BU)
#define ADC_REGULAR_RANK_12   (0x0000000CU)
#define ADC_REGULAR_RANK_13   (0x0000000DU)
#define ADC_REGULAR_RANK_14   (0x0000000EU)
#define ADC_REGULAR_RANK_15   (0x0000000FU)
#define ADC_REGULAR_RANK_16   (0x00000010U)
#define ADC_REGULAR_RANK_17   (0x00000011U)
#define ADC_REGULAR_RANK_18   (0x00000012U)
#define ADC_REGULAR_RANK_19   (0x00000013U)
#define ADC_REGULAR_RANK_20   (0x00000014U)
#define ADC_REGULAR_RANK_21   (0x00000015U)
#define ADC_REGULAR_RANK_22   (0x00000016U)
#define ADC_REGULAR_RANK_23   (0x00000017U)
#define ADC_REGULAR_RANK_24   (0x00000018U)
#define ADC_REGULAR_RANK_25   (0x00000019U)
#define ADC_REGULAR_RANK_26   (0x0000001AU)
#define ADC_REGULAR_RANK_27   (0x0000001BU)
#if defined(STM32L100xC) || defined (STM32L151xC) || defined (STM32L152xC) || defined (STM32L162xC) || defined(STM32L151xCA) || defined (STM32L151xD) || defined (STM32L152xCA) || defined (STM32L152xD) || defined (STM32L162xCA) || defined (STM32L162xD) || defined(STM32L151xE) || defined(STM32L151xDX) || defined (STM32L152xE) || defined (STM32L152xDX) || defined (STM32L162xE) || defined (STM32L162xDX)
#define ADC_REGULAR_RANK_28   (0x0000001CU)
#endif /* STM32L100xC || STM32L151xC || STM32L152xC || STM32L162xC || STM32L151xCA || STM32L151xD || STM32L152xCA || STM32L152xD || STM32L162xCA || STM32L162xD || STM32L151xE || STM32L151xDX || STM32L152xE || STM32L152xDX || STM32L162xE || STM32L162xDX */

#define VREFINT_MV 1224

unsigned int ADC_Init(ADC_TypeDef *instance, ADC_InitTypeDef *init);
void ADC_StartConversion(ADC_TypeDef *instance);
void ADC_Enable(ADC_TypeDef *instance);
void ADC_ConfigChannel(ADC_TypeDef* instance, ADC_ChannelConfTypeDef* sConfig);
unsigned int ADC_GetValue(ADC_TypeDef* instance);
unsigned int ADC_PollForConversion(ADC_TypeDef* instance, unsigned int timeout);

#endif
