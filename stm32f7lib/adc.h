#ifndef _ADC_H
#define _ADC_H

#include <board.h>
#include <dma.h>

#define ADC_EXTERNALTRIGCONV_T1_CC1    ((unsigned int)0x00000000U)
#define ADC_EXTERNALTRIGCONV_T1_CC2    ((unsigned int)ADC_CR2_EXTSEL_0)
#define ADC_EXTERNALTRIGCONV_T1_CC3    ((unsigned int)ADC_CR2_EXTSEL_1)
#define ADC_EXTERNALTRIGCONV_T2_CC2    ((unsigned int)(ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T5_TRGO   ((unsigned int)ADC_CR2_EXTSEL_2)
#define ADC_EXTERNALTRIGCONV_T4_CC4    ((unsigned int)(ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T3_CC4    ((unsigned int)(ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1))
#define ADC_EXTERNALTRIGCONV_T8_TRGO   ((unsigned int)(ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T8_TRGO2  ((unsigned int)ADC_CR2_EXTSEL_3)
#define ADC_EXTERNALTRIGCONV_T1_TRGO   ((unsigned int)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T1_TRGO2  ((unsigned int)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_1))
#define ADC_EXTERNALTRIGCONV_T2_TRGO   ((unsigned int)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0))
#define ADC_EXTERNALTRIGCONV_T4_TRGO   ((unsigned int)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_2))
#define ADC_EXTERNALTRIGCONV_T6_TRGO   ((unsigned int)(ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_0))

#define ADC_EXTERNALTRIGCONV_EXT_IT11  ((unsigned int)ADC_CR2_EXTSEL)
#define ADC_SOFTWARE_START             ((unsigned int)ADC_CR2_EXTSEL + 1)

#define ADC_CLOCK_SYNC_PCLK_DIV2    ((unsigned int)0x00000000U)
#define ADC_CLOCK_SYNC_PCLK_DIV4    ((unsigned int)ADC_CCR_ADCPRE_0)
#define ADC_CLOCK_SYNC_PCLK_DIV6    ((unsigned int)ADC_CCR_ADCPRE_1)
#define ADC_CLOCK_SYNC_PCLK_DIV8    ((unsigned int)ADC_CCR_ADCPRE)

#define ADC_RESOLUTION_12B  ((unsigned int)0x00000000U)
#define ADC_RESOLUTION_10B  ((unsigned int)ADC_CR1_RES_0)
#define ADC_RESOLUTION_8B   ((unsigned int)ADC_CR1_RES_1)
#define ADC_RESOLUTION_6B   ((unsigned int)ADC_CR1_RES)

#define ADC_EXTERNALTRIGCONVEDGE_NONE           ((unsigned int)0x00000000U)
#define ADC_EXTERNALTRIGCONVEDGE_RISING         ((unsigned int)ADC_CR2_EXTEN_0)
#define ADC_EXTERNALTRIGCONVEDGE_FALLING        ((unsigned int)ADC_CR2_EXTEN_1)
#define ADC_EXTERNALTRIGCONVEDGE_RISINGFALLING  ((unsigned int)ADC_CR2_EXTEN)

#define ADC_DATAALIGN_RIGHT      ((unsigned int)0x00000000U)
#define ADC_DATAALIGN_LEFT       ((unsigned int)ADC_CR2_ALIGN)

#define ADC_CHANNEL_0           ((unsigned int)0x00000000U)
#define ADC_CHANNEL_1           ((unsigned int)ADC_CR1_AWDCH_0)
#define ADC_CHANNEL_2           ((unsigned int)ADC_CR1_AWDCH_1)
#define ADC_CHANNEL_3           ((unsigned int)(ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_4           ((unsigned int)ADC_CR1_AWDCH_2)
#define ADC_CHANNEL_5           ((unsigned int)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_6           ((unsigned int)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_7           ((unsigned int)(ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_8           ((unsigned int)ADC_CR1_AWDCH_3)
#define ADC_CHANNEL_9           ((unsigned int)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_10          ((unsigned int)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_11          ((unsigned int)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_12          ((unsigned int)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2))
#define ADC_CHANNEL_13          ((unsigned int)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_14          ((unsigned int)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1))
#define ADC_CHANNEL_15          ((unsigned int)(ADC_CR1_AWDCH_3 | ADC_CR1_AWDCH_2 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_16          ((unsigned int)ADC_CR1_AWDCH_4)
#define ADC_CHANNEL_17          ((unsigned int)(ADC_CR1_AWDCH_4 | ADC_CR1_AWDCH_0))
#define ADC_CHANNEL_18          ((unsigned int)(ADC_CR1_AWDCH_4 | ADC_CR1_AWDCH_1))

#define ADC_CHANNEL_VREFINT     ((unsigned int)ADC_CHANNEL_17)
#define ADC_CHANNEL_VBAT        ((unsigned int)ADC_CHANNEL_18)

#define ADC_CHANNEL_TEMPSENSOR  ((unsigned int)ADC_CHANNEL_18 | 0x10000000U)

#define ADC_SAMPLETIME_3CYCLES    ((unsigned int)0x00000000U)
#define ADC_SAMPLETIME_15CYCLES   ((unsigned int)ADC_SMPR1_SMP10_0)
#define ADC_SAMPLETIME_28CYCLES   ((unsigned int)ADC_SMPR1_SMP10_1)
#define ADC_SAMPLETIME_56CYCLES   ((unsigned int)(ADC_SMPR1_SMP10_1 | ADC_SMPR1_SMP10_0))
#define ADC_SAMPLETIME_84CYCLES   ((unsigned int)ADC_SMPR1_SMP10_2)
#define ADC_SAMPLETIME_112CYCLES  ((unsigned int)(ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_0))
#define ADC_SAMPLETIME_144CYCLES  ((unsigned int)(ADC_SMPR1_SMP10_2 | ADC_SMPR1_SMP10_1))
#define ADC_SAMPLETIME_480CYCLES  ((unsigned int)ADC_SMPR1_SMP10)

#define ADC_FLAG_AWD    ((unsigned int)ADC_SR_AWD)
#define ADC_FLAG_EOC    ((unsigned int)ADC_SR_EOC)
#define ADC_FLAG_JEOC   ((unsigned int)ADC_SR_JEOC)
#define ADC_FLAG_JSTRT  ((unsigned int)ADC_SR_JSTRT)
#define ADC_FLAG_STRT   ((unsigned int)ADC_SR_STRT)
#define ADC_FLAG_OVR    ((unsigned int)ADC_SR_OVR)

#define ADC_IT_EOC      ((unsigned int)ADC_CR1_EOCIE)  
#define ADC_IT_AWD      ((unsigned int)ADC_CR1_AWDIE) 
#define ADC_IT_JEOC     ((unsigned int)ADC_CR1_JEOCIE)
#define ADC_IT_OVR      ((unsigned int)ADC_CR1_OVRIE) 

#define ADC_EOC_SEQ_CONV              ((uint32_t)0x00000000U)
#define ADC_EOC_SINGLE_CONV           ((uint32_t)0x00000001U)

/**
  * @brief  Enable the ADC peripheral.
  * @param  __HANDLE__ ADC handle
  * @retval None
  */
#define ADC_ENABLE(__ADC__) ((__ADC__)->CR2 |=  ADC_CR2_ADON)

/**
  * @brief  Disable the ADC peripheral.
  * @param  __HANDLE__ ADC handle
  * @retval None
  */
#define ADC_DISABLE(__ADC__) ((__ADC__)->CR2 &=  ~ADC_CR2_ADON)

#define ADC_ENABLE_IT(__ADC__, __INTERRUPT__) (((__ADC__)->CR1) |= (__INTERRUPT__))

#define ADC_CLEAR_FLAG(__ADC__, __FLAG__) (((__ADC__)->SR) = ~(__FLAG__))

/** 
  * @brief  Structure definition of ADC and regular group initialization 
  * @note   Parameters of this structure are shared within 2 scopes:
  *          - Scope entire ADC (affects regular and injected groups): ClockPrescaler, Resolution, ScanConvMode, DataAlign, ScanConvMode, EOCSelection, LowPowerAutoWait, LowPowerAutoPowerOff, ChannelsBank.
  *          - Scope regular group: ContinuousConvMode, NbrOfConversion, DiscontinuousConvMode, NbrOfDiscConversion, ExternalTrigConvEdge, ExternalTrigConv.
  * @note   The setting of these parameters with function HAL_ADC_Init() is conditioned to ADC state.
  *         ADC state can be either:
  *          - For all parameters: ADC disabled
  *          - For all parameters except 'Resolution', 'ScanConvMode', 'DiscontinuousConvMode', 'NbrOfDiscConversion' : ADC enabled without conversion on going on regular group.
  *          - For parameters 'ExternalTrigConv' and 'ExternalTrigConvEdge': ADC enabled, even with conversion on going.
  *         If ADC is not in the appropriate state to modify some parameters, these parameters setting is bypassed
  *         without error reporting (as it can be the expected behaviour in case of intended action to update another parameter (which fullfills the ADC state condition) on the fly).
  */
typedef struct
{
  unsigned int ClockPrescaler;               /*!< Select ADC clock prescaler. The clock is common for 
                                              all the ADCs.
                                              This parameter can be a value of @ref ADC_ClockPrescaler */
  unsigned int Resolution;                   /*!< Configures the ADC resolution.
                                              This parameter can be a value of @ref ADC_Resolution */
  unsigned int DataAlign;                    /*!< Specifies ADC data alignment to right (MSB on register bit 11 and LSB on register bit 0) (default setting)
                                              or to left (if regular group: MSB on register bit 15 and LSB on register bit 4, if injected group (MSB kept as signed value due to potential negative value after offset application): MSB on register bit 14 and LSB on register bit 3).
                                              This parameter can be a value of @ref ADC_Data_Align */
  unsigned int ScanConvMode;                 /*!< Configures the sequencer of regular and injected groups.
                                              This parameter can be associated to parameter 'DiscontinuousConvMode' to have main sequence subdivided in successive parts.
                                              If disabled: Conversion is performed in single mode (one channel converted, the one defined in rank 1).
                                                           Parameters 'NbrOfConversion' and 'InjectedNbrOfConversion' are discarded (equivalent to set to 1).
                                              If enabled:  Conversions are performed in sequence mode (multiple ranks defined by 'NbrOfConversion'/'InjectedNbrOfConversion' and each channel rank).
                                                           Scan direction is upward: from rank1 to rank 'n'.
                                              This parameter can be a value of @ref ADC_Scan_mode.
                                              This parameter can be set to ENABLE or DISABLE */
  unsigned int EOCSelection;                 /*!< Specifies what EOC (End Of Conversion) flag is used for conversion by polling and interruption: end of conversion of each rank or complete sequence.
                                              This parameter can be a value of @ref ADC_EOCSelection.
                                              Note: For injected group, end of conversion (flag&IT) is raised only at the end of the sequence.
                                                    Therefore, if end of conversion is set to end of each conversion, injected group should not be used with interruption (HAL_ADCEx_InjectedStart_IT)
                                                    or polling (HAL_ADCEx_InjectedStart and HAL_ADCEx_InjectedPollForConversion). By the way, polling is still possible since driver will use an estimated timing for end of injected conversion.
                                              Note: If overrun feature is intended to be used, use ADC in mode 'interruption' (function HAL_ADC_Start_IT() ) with parameter EOCSelection set to end of each conversion or in mode 'transfer by DMA' (function HAL_ADC_Start_DMA()).
                                              If overrun feature is intended to be bypassed, use ADC in mode 'polling' or 'interruption' with parameter EOCSelection must be set to end of sequence */
  unsigned int ContinuousConvMode;           /*!< Specifies whether the conversion is performed in single mode (one conversion) or continuous mode for regular group,
                                              after the selected trigger occurred (software start or external trigger).
                                              This parameter can be set to ENABLE or DISABLE. */
  unsigned int NbrOfConversion;              /*!< Specifies the number of ranks that will be converted within the regular group sequencer.
                                              To use regular group sequencer and convert several ranks, parameter 'ScanConvMode' must be enabled.
                                              This parameter must be a number between Min_Data = 1 and Max_Data = 16. */
  FunctionalState DiscontinuousConvMode; /*!< Specifies whether the conversions sequence of regular group is performed in Complete-sequence/Discontinuous-sequence (main sequence subdivided in successive parts).
                                              Discontinuous mode is used only if sequencer is enabled (parameter 'ScanConvMode'). If sequencer is disabled, this parameter is discarded.
                                              Discontinuous mode can be enabled only if continuous mode is disabled. If continuous mode is enabled, this parameter setting is discarded.
                                              This parameter can be set to ENABLE or DISABLE. */
  unsigned int NbrOfDiscConversion;          /*!< Specifies the number of discontinuous conversions in which the  main sequence of regular group (parameter NbrOfConversion) will be subdivided.
                                              If parameter 'DiscontinuousConvMode' is disabled, this parameter is discarded.
                                              This parameter must be a number between Min_Data = 1 and Max_Data = 8. */
  unsigned int ExternalTrigConv;             /*!< Selects the external event used to trigger the conversion start of regular group.
                                              If set to ADC_SOFTWARE_START, external triggers are disabled.
                                              If set to external trigger source, triggering is on event rising edge by default.
                                              This parameter can be a value of @ref ADC_External_trigger_Source_Regular */
  unsigned int ExternalTrigConvEdge;         /*!< Selects the external trigger edge of regular group.
                                              If trigger is set to ADC_SOFTWARE_START, this parameter is discarded.
                                              This parameter can be a value of @ref ADC_External_trigger_edge_Regular */
  FunctionalState DMAContinuousRequests; /*!< Specifies whether the DMA requests are performed in one shot mode (DMA transfer stop when number of conversions is reached)
                                              or in Continuous mode (DMA transfer unlimited, whatever number of conversions).
                                              Note: In continuous mode, DMA must be configured in circular mode. Otherwise an overrun will be triggered when DMA buffer maximum pointer is reached.
                                              Note: This parameter must be modified when no conversion is on going on both regular and injected groups (ADC disabled, or ADC enabled without continuous mode or external trigger that could launch a conversion).
                                              This parameter can be set to ENABLE or DISABLE. */
} ADC_InitTypeDef;

/**
  * @brief  Structure definition of ADC channel for regular group
  * @note   The setting of these parameters with function HAL_ADC_ConfigChannel() is conditioned to ADC state.
  *         ADC can be either disabled or enabled without conversion on going on regular group.
  */
typedef struct
{
  unsigned int Channel;                /*!< Specifies the channel to configure into ADC regular group.
                                        This parameter can be a value of @ref ADC_channels */
  unsigned int Rank;                   /*!< Specifies the rank in the regular group sequencer.
                                        This parameter must be a number between Min_Data = 1 and Max_Data = 16
                                        This parameter can be a value of @ref ADC_regular_rank */
  unsigned int SamplingTime;           /*!< Sampling time value to be set for the selected channel.
                                        Unit: ADC clock cycles
                                        Conversion time is the addition of sampling time and processing time (12 ADC clock cycles at ADC resolution 12 bits, 11 cycles at 10 bits, 9 cycles at 8 bits, 7 cycles at 6 bits).
                                        This parameter can be a value of @ref ADC_sampling_times
                                        Caution: This parameter updates the parameter property of the channel, that can be used into regular and/or injected groups.
                                                 If this same channel has been previously configured in the other group (regular/injected), it will be updated to last setting.
                                        Note: In case of usage of internal measurement channels (VrefInt/Vbat/TempSensor),
                                              sampling time constraints must be respected (sampling time can be adjusted in function of ADC clock frequency and sampling time setting)
                                              Refer to device datasheet for timings values, parameters TS_vrefint, TS_temp (values rough order: 4us min). */
  unsigned int Offset;                 /*!< Reserved for future use, can be set to 0 */
} ADC_ChannelConfTypeDef;

void ADC_Init(ADC_TypeDef *adc, ADC_InitTypeDef* init);
void ADC_ConfigChannel(ADC_TypeDef *adc, ADC_ChannelConfTypeDef* sConfig);
void ADC_Start_DMA(ADC_TypeDef *adc, DMA_HandleTypeDef *hdma, unsigned short* pData, unsigned short* pData2, unsigned int Length);

#endif
