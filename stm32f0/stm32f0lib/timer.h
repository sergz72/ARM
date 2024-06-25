#ifndef _TIM_H
#define _TIM_H

#include "stm32f0xx.h"

typedef struct
{
  uint16_t TIM_Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
                                       This parameter can be a number between 0x0000 and 0xFFFF */

  uint16_t TIM_CounterMode;       /*!< Specifies the counter mode.
                                       This parameter can be a value of @ref TIM_Counter_Mode */

  uint32_t TIM_Period;            /*!< Specifies the period value to be loaded into the active
                                       Auto-Reload Register at the next update event.
                                       This parameter must be a number between 0x0000 and 0xFFFF.  */ 

  uint16_t TIM_ClockDivision;     /*!< Specifies the clock division.
                                      This parameter can be a value of @ref TIM_Clock_Division_CKD */

  uint8_t TIM_RepetitionCounter;  /*!< Specifies the repetition counter value. Each time the RCR downcounter
                                       reaches zero, an update event is generated and counting restarts
                                       from the RCR value (N).
                                       This means in PWM mode that (N+1) corresponds to:
                                          - the number of PWM periods in edge-aligned mode
                                          - the number of half PWM period in center-aligned mode
                                       This parameter must be a number between 0x00 and 0xFF. 
                                       @note This parameter is valid only for TIM1. */
} TIM_TimeBaseInitTypeDef;       

/** 
  * @brief  TIM Output Compare Init structure definition  
  */

/**
  * @brief  TIM Output Compare Configuration Structure definition
  */
typedef struct
{
  uint32_t OCMode;        /*!< Specifies the TIM mode.
                               This parameter can be a value of @ref TIM_Output_Compare_and_PWM_modes */

  uint32_t OutputState;   /*!< Specifies the TIM Output Compare state.
                                   This parameter can be a value of @ref TIM_Output_Compare_state */

  uint32_t OutputNState;  /*!< Specifies the TIM complementary Output Compare state.
                                   This parameter can be a value of @ref TIM_Output_Compare_N_state
                                   @note This parameter is valid only for TIM1. */

  uint32_t Pulse;         /*!< Specifies the pulse value to be loaded into the Capture Compare Register.
                               This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */

  uint32_t OCPolarity;    /*!< Specifies the output polarity.
                               This parameter can be a value of @ref TIM_Output_Compare_Polarity */

  uint32_t OCNPolarity;   /*!< Specifies the complementary output polarity.
                               This parameter can be a value of @ref TIM_Output_Compare_N_Polarity
                               @note This parameter is valid only for timer instances supporting break feature. */

  uint32_t OCFastMode;    /*!< Specifies the Fast mode state.
                               This parameter can be a value of @ref TIM_Output_Fast_State
                               @note This parameter is valid only in PWM1 and PWM2 mode. */


  uint32_t OCIdleState;   /*!< Specifies the TIM Output Compare pin state during Idle state.
                               This parameter can be a value of @ref TIM_Output_Compare_Idle_State
                               @note This parameter is valid only for timer instances supporting break feature. */

  uint32_t OCNIdleState;  /*!< Specifies the TIM Output Compare pin state during Idle state.
                               This parameter can be a value of @ref TIM_Output_Compare_N_Idle_State
                               @note This parameter is valid only for timer instances supporting break feature. */
} TIM_OC_InitTypeDef;

#define TIM_PSCReloadMode_Update           ((uint16_t)0x0000)
#define TIM_PSCReloadMode_Immediate        ((uint16_t)0x0001)

#define TIM_CounterMode_Up                 ((uint16_t)0x0000)
#define TIM_CounterMode_Down               ((uint16_t)0x0010)
#define TIM_CounterMode_CenterAligned1     ((uint16_t)0x0020)
#define TIM_CounterMode_CenterAligned2     ((uint16_t)0x0040)
#define TIM_CounterMode_CenterAligned3     ((uint16_t)0x0060)

#define TIM_OCMode_Frozen                  ((uint16_t)0x0000)
#define TIM_OCMode_Active                  ((uint16_t)0x0010)
#define TIM_OCMode_Inactive                ((uint16_t)0x0020)
#define TIM_OCMode_Toggle                  ((uint16_t)0x0030)
#define TIM_OCMode_INACTLevel              ((uint16_t)0x0040)
#define TIM_OCMode_ACTLevel                ((uint16_t)0x0050)
#define TIM_OCMode_PWM1                    ((uint16_t)0x0060)
#define TIM_OCMode_PWM2                    ((uint16_t)0x0070)

#define TIM_OCPolarity_High                ((uint16_t)0x0000)
#define TIM_OCPolarity_Low                 ((uint16_t)0x0002)

#define TIM_OutputState_Disable            ((uint16_t)0x0000)
#define TIM_OutputState_Enable             ((uint16_t)0x0001)

#define TIM_OCPreload_Enable               ((uint16_t)0x0008)
#define TIM_OCPreload_Disable              ((uint16_t)0x0000)

#define TIM_IT_Update                      ((uint16_t)0x0001)
#define TIM_IT_CC1                         ((uint16_t)0x0002)
#define TIM_IT_CC2                         ((uint16_t)0x0004)
#define TIM_IT_CC3                         ((uint16_t)0x0008)
#define TIM_IT_CC4                         ((uint16_t)0x0010)
#define TIM_IT_COM                         ((uint16_t)0x0020)
#define TIM_IT_Trigger                     ((uint16_t)0x0040)
#define TIM_IT_Break                       ((uint16_t)0x0080)

void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_ARRPreloadConfig(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState);
void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT);
ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT);
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_OC1_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_OC3_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_OC4_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);

void TIM_OC1PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC2PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC3PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);
void TIM_OC4PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);

void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState);

void TIM_SetCompare1(TIM_TypeDef* TIMx, unsigned int Compare1);
void TIM_SetCompare2(TIM_TypeDef* TIMx, unsigned int Compare2);
void TIM_SetCompare3(TIM_TypeDef* TIMx, unsigned int Compare3);
void TIM_SetCompare4(TIM_TypeDef* TIMx, unsigned int Compare4);

#endif /*_TIM_H */
