#ifndef _ADC_H
#define _ADC_H

#include "stm32f0xx.h"

/** 
  * @brief  ADC Init structure definition
  */
  
typedef struct
{
  uint32_t ADC_Resolution;                  /*!< Selects the resolution of the conversion.
                                                 This parameter can be a value of @ref ADC_Resolution */

  FunctionalState ADC_ContinuousConvMode;   /*!< Specifies whether the conversion is performed in
                                                 Continuous or Single mode.
                                                 This parameter can be set to ENABLE or DISABLE. */

  uint32_t ADC_ExternalTrigConvEdge;        /*!< Selects the external trigger Edge and enables the
                                                 trigger of a regular group. This parameter can be a value
                                                 of @ref ADC_external_trigger_edge_conversion */

  uint32_t ADC_ExternalTrigConv;            /*!< Defines the external trigger used to start the analog
                                                 to digital conversion of regular channels. This parameter
                                                 can be a value of @ref ADC_external_trigger_sources_for_channels_conversion */

  uint32_t ADC_DataAlign;                   /*!< Specifies whether the ADC data alignment is left or right.
                                                 This parameter can be a value of @ref ADC_data_align */

  uint32_t  ADC_ScanDirection;              /*!< Specifies in which direction the channels will be scanned
                                                 in the sequence. 
                                                 This parameter can be a value of @ref ADC_Scan_Direction */
} ADC_InitTypeDef;

#define ADC_ClockMode_AsynClk                  ((uint32_t)0x00000000)   /*!< ADC Asynchronous clock mode */
#define ADC_ClockMode_SynClkDiv2               ADC_CFGR2_CKMODE_0   /*!<  Synchronous clock mode divided by 2 */
#define ADC_ClockMode_SynClkDiv4               ADC_CFGR2_CKMODE_1   /*!<  Synchronous clock mode divided by 4 */

#define ADC_Resolution_12b                         ((uint32_t)0x00000000)
#define ADC_Resolution_10b                         ADC_CFGR1_RES_0
#define ADC_Resolution_8b                          ADC_CFGR1_RES_1
#define ADC_Resolution_6b                          ADC_CFGR1_RES

#define ADC_ExternalTrigConvEdge_None              ((uint32_t)0x00000000)
#define ADC_ExternalTrigConvEdge_Rising            ADC_CFGR1_EXTEN_0
#define ADC_ExternalTrigConvEdge_Falling           ADC_CFGR1_EXTEN_1
#define ADC_ExternalTrigConvEdge_RisingFalling     ADC_CFGR1_EXTEN

/* TIM1 */
#define ADC_ExternalTrigConv_T1_TRGO               ((uint32_t)0x00000000)
#define ADC_ExternalTrigConv_T1_CC4                ADC_CFGR1_EXTSEL_0

/* TIM2 */
#define ADC_ExternalTrigConv_T2_TRGO               ADC_CFGR1_EXTSEL_1

/* TIM3 */
#define ADC_ExternalTrigConv_T3_TRGO               ((uint32_t)(ADC_CFGR1_EXTSEL_0 | ADC_CFGR1_EXTSEL_1))

/* TIM15 */
#define ADC_ExternalTrigConv_T15_TRGO              ADC_CFGR1_EXTSEL_2

#define ADC_DataAlign_Right                        ((uint32_t)0x00000000)
#define ADC_DataAlign_Left                         ADC_CFGR1_ALIGN
  
#define ADC_ScanDirection_Upward                   ((uint32_t)0x00000000)
#define ADC_ScanDirection_Backward                 ADC_CFGR1_SCANDIR
  
#define ADC_DMAMode_OneShot                        ((uint32_t)0x00000000)
#define ADC_DMAMode_Circular                       ADC_CFGR1_DMACFG
  
#define ADC_AnalogWatchdog_Channel_0                 ((uint32_t)0x00000000)
#define ADC_AnalogWatchdog_Channel_1                 ((uint32_t)0x04000000)
#define ADC_AnalogWatchdog_Channel_2                 ((uint32_t)0x08000000)
#define ADC_AnalogWatchdog_Channel_3                 ((uint32_t)0x0C000000)
#define ADC_AnalogWatchdog_Channel_4                 ((uint32_t)0x10000000)
#define ADC_AnalogWatchdog_Channel_5                 ((uint32_t)0x14000000)
#define ADC_AnalogWatchdog_Channel_6                 ((uint32_t)0x18000000)
#define ADC_AnalogWatchdog_Channel_7                 ((uint32_t)0x1C000000)
#define ADC_AnalogWatchdog_Channel_8                 ((uint32_t)0x20000000)
#define ADC_AnalogWatchdog_Channel_9                 ((uint32_t)0x24000000)
#define ADC_AnalogWatchdog_Channel_10                ((uint32_t)0x28000000) /*!< Not available for STM32F031 devices */
#define ADC_AnalogWatchdog_Channel_11                ((uint32_t)0x2C000000) /*!< Not available for STM32F031 devices */
#define ADC_AnalogWatchdog_Channel_12                ((uint32_t)0x30000000) /*!< Not available for STM32F031 devices */
#define ADC_AnalogWatchdog_Channel_13                ((uint32_t)0x34000000) /*!< Not available for STM32F031 devices */
#define ADC_AnalogWatchdog_Channel_14                ((uint32_t)0x38000000) /*!< Not available for STM32F031 devices */
#define ADC_AnalogWatchdog_Channel_15                ((uint32_t)0x3C000000) /*!< Not available for STM32F031 devices */
#define ADC_AnalogWatchdog_Channel_16                ((uint32_t)0x40000000)
#define ADC_AnalogWatchdog_Channel_17                ((uint32_t)0x44000000)
#define ADC_AnalogWatchdog_Channel_18                ((uint32_t)0x48000000)

#define ADC_SampleTime_1_5Cycles                     ((uint32_t)0x00000000)
#define ADC_SampleTime_7_5Cycles                     ((uint32_t)0x00000001)
#define ADC_SampleTime_13_5Cycles                    ((uint32_t)0x00000002)
#define ADC_SampleTime_28_5Cycles                    ((uint32_t)0x00000003)
#define ADC_SampleTime_41_5Cycles                    ((uint32_t)0x00000004)
#define ADC_SampleTime_55_5Cycles                    ((uint32_t)0x00000005)
#define ADC_SampleTime_71_5Cycles                    ((uint32_t)0x00000006)
#define ADC_SampleTime_239_5Cycles                   ((uint32_t)0x00000007)
  
#define ADC_Channel_0                              ADC_CHSELR_CHSEL0
#define ADC_Channel_1                              ADC_CHSELR_CHSEL1
#define ADC_Channel_2                              ADC_CHSELR_CHSEL2
#define ADC_Channel_3                              ADC_CHSELR_CHSEL3
#define ADC_Channel_4                              ADC_CHSELR_CHSEL4
#define ADC_Channel_5                              ADC_CHSELR_CHSEL5
#define ADC_Channel_6                              ADC_CHSELR_CHSEL6
#define ADC_Channel_7                              ADC_CHSELR_CHSEL7
#define ADC_Channel_8                              ADC_CHSELR_CHSEL8
#define ADC_Channel_9                              ADC_CHSELR_CHSEL9
#define ADC_Channel_10                             ADC_CHSELR_CHSEL10 /*!< Not available for STM32F031 devices */
#define ADC_Channel_11                             ADC_CHSELR_CHSEL11 /*!< Not available for STM32F031 devices */
#define ADC_Channel_12                             ADC_CHSELR_CHSEL12 /*!< Not available for STM32F031 devices */
#define ADC_Channel_13                             ADC_CHSELR_CHSEL13 /*!< Not available for STM32F031 devices */
#define ADC_Channel_14                             ADC_CHSELR_CHSEL14 /*!< Not available for STM32F031 devices */
#define ADC_Channel_15                             ADC_CHSELR_CHSEL15 /*!< Not available for STM32F031 devices */
#define ADC_Channel_16                             ADC_CHSELR_CHSEL16
#define ADC_Channel_17                             ADC_CHSELR_CHSEL17
#define ADC_Channel_18                             ADC_CHSELR_CHSEL18 /*!< Not available for STM32F030 devices */

#define ADC_Channel_TempSensor                     ((uint32_t)ADC_Channel_16)
#define ADC_Channel_Vrefint                        ((uint32_t)ADC_Channel_17)
#define ADC_Channel_Vbat                           ((uint32_t)ADC_Channel_18) /*!< Not available for STM32F030 devices */
  
#define ADC_IT_ADRDY                               ADC_IER_ADRDYIE
#define ADC_IT_EOSMP                               ADC_IER_EOSMPIE
#define ADC_IT_EOC                                 ADC_IER_EOCIE
#define ADC_IT_EOSEQ                               ADC_IER_EOSEQIE
#define ADC_IT_OVR                                 ADC_IER_OVRIE
#define ADC_IT_AWD                                 ADC_IER_AWDIE
  
#define ADC_FLAG_ADRDY                             ADC_ISR_ADRDY
#define ADC_FLAG_EOSMP                             ADC_ISR_EOSMP
#define ADC_FLAG_EOC                               ADC_ISR_EOC
#define ADC_FLAG_EOSEQ                             ADC_ISR_EOSEQ
#define ADC_FLAG_OVR                               ADC_ISR_OVR
#define ADC_FLAG_AWD                               ADC_ISR_AWD

#define ADC_FLAG_ADEN                              ((uint32_t)0x01000001)
#define ADC_FLAG_ADDIS                             ((uint32_t)0x01000002)
#define ADC_FLAG_ADSTART                           ((uint32_t)0x01000004)
#define ADC_FLAG_ADSTP                             ((uint32_t)0x01000010)
#define ADC_FLAG_ADCAL                             ((uint32_t)0x81000000) 

/* Exported functions ------------------------------------------------------- */ 

/*  Function used to set the ADC configuration to the default reset state *****/
void ADC_DeInit(ADC_TypeDef* ADCx);

/* Initialization and Configuration functions *********************************/ 
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct);
void ADC_ClockModeConfig(ADC_TypeDef* ADCx, uint32_t ADC_ClockMode);
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);
/* This Function is obsolete and maintained for legacy purpose only.
   ADC_ClockModeConfig() function should be used instead */
void ADC_JitterCmd(ADC_TypeDef* ADCx, uint32_t ADC_JitterOff, FunctionalState NewState);

/* Power saving functions *****************************************************/
void ADC_AutoPowerOffCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_WaitModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/* Analog Watchdog configuration functions ************************************/
void ADC_AnalogWatchdogCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef* ADCx, uint16_t HighThreshold,uint16_t LowThreshold);
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef* ADCx, uint32_t ADC_AnalogWatchdog_Channel);
void ADC_AnalogWatchdogSingleChannelCmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/* Temperature Sensor , Vrefint and Vbat management function ******************/
void ADC_TempSensorCmd(FunctionalState NewState);
void ADC_VrefintCmd(FunctionalState NewState);
void ADC_VbatCmd(FunctionalState NewState); /*!< Not applicable for STM32F030 devices */

/* Channels Configuration functions *******************************************/
void ADC_ChannelConfig(ADC_TypeDef* ADCx, uint32_t ADC_Channel, uint32_t ADC_SampleTime);
void ADC_ContinuousModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_DiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_OverrunModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
uint32_t ADC_GetCalibrationFactor(ADC_TypeDef* ADCx);
void ADC_StopOfConversion(ADC_TypeDef* ADCx);
void ADC_StartOfConversion(ADC_TypeDef* ADCx);
unsigned int ADC_GetConversionValue(ADC_TypeDef* ADCx);

/* Regular Channels DMA Configuration functions *******************************/
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_DMARequestModeConfig(ADC_TypeDef* ADCx, uint32_t ADC_DMARequestMode);

/* Interrupts and flags management functions **********************************/
void ADC_ITConfig(ADC_TypeDef* ADCx, uint32_t ADC_IT, FunctionalState NewState);
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint32_t ADC_FLAG);
void ADC_ClearFlag(ADC_TypeDef* ADCx, uint32_t ADC_FLAG);
ITStatus ADC_GetITStatus(ADC_TypeDef* ADCx, uint32_t ADC_IT);
void ADC_ClearITPendingBit(ADC_TypeDef* ADCx, uint32_t ADC_IT);

#endif /*_ADC_H */
