#ifndef _TIM_H
#define _TIM_H

#include <stm32g0xx.h>

typedef struct
{
  unsigned short TIM_Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
                                       This parameter can be a number between 0x0000 and 0xFFFF */

  unsigned short TIM_CounterMode;       /*!< Specifies the counter mode.
                                       This parameter can be a value of @ref TIM_Counter_Mode */

  unsigned int TIM_Period;            /*!< Specifies the period value to be loaded into the active
                                       Auto-Reload Register at the next update event.
                                       This parameter must be a number between 0x0000 and 0xFFFF.  */ 

  unsigned short TIM_ClockDivision;     /*!< Specifies the clock division.
                                      This parameter can be a value of @ref TIM_Clock_Division_CKD */
} TIM_TimeBaseInitTypeDef;

/** 
  * @brief  TIM Output Compare Init structure definition  
  */

/**
  * @brief  TIM Output Compare Configuration Structure definition
  */
typedef struct
{
  unsigned int OCMode;        /*!< Specifies the TIM mode.
                               This parameter can be a value of @ref TIM_Output_Compare_and_PWM_modes */

  unsigned int OutputState;   /*!< Specifies the TIM Output Compare state.
                                   This parameter can be a value of @ref TIM_Output_Compare_state */

  unsigned int Pulse;         /*!< Specifies the pulse value to be loaded into the Capture Compare Register.
                               This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */

  unsigned int OCPolarity;    /*!< Specifies the output polarity.
                               This parameter can be a value of @ref TIM_Output_Compare_Polarity */

  unsigned int OCNPolarity;   /*!< Specifies the complementary output polarity.
                               This parameter can be a value of @ref TIM_Output_Compare_N_Polarity
                               @note This parameter is valid only for timer instances supporting break feature. */
} TIM_OC_InitTypeDef;

#define TIM_PSCReloadMode_Update           ((unsigned short)0x0000)
#define TIM_PSCReloadMode_Immediate        ((unsigned short)0x0001)

#define TIM_CounterMode_Up                 ((unsigned short)0x0000)
#define TIM_CounterMode_Down               ((unsigned short)0x0010)
#define TIM_CounterMode_CenterAligned1     ((unsigned short)0x0020)
#define TIM_CounterMode_CenterAligned2     ((unsigned short)0x0040)
#define TIM_CounterMode_CenterAligned3     ((unsigned short)0x0060)

#define TIM_OCMode_Frozen                  ((unsigned short)0x0000)
#define TIM_OCMode_Active                  ((unsigned short)0x0010)
#define TIM_OCMode_Inactive                ((unsigned short)0x0020)
#define TIM_OCMode_Toggle                  ((unsigned short)0x0030)
#define TIM_OCMode_INACTLevel              ((unsigned short)0x0040)
#define TIM_OCMode_ACTLevel                ((unsigned short)0x0050)
#define TIM_OCMode_PWM1                    ((unsigned short)0x0060)
#define TIM_OCMode_PWM2                    ((unsigned short)0x0070)

#define TIM_OCPolarity_High                ((unsigned short)0x0000)
#define TIM_OCPolarity_Low                 ((unsigned short)0x0002)

#define TIM_OutputState_Disable            ((unsigned short)0x0000)
#define TIM_OutputState_Enable             ((unsigned short)0x0001)

#define TIM_OCPreload_Enable               ((unsigned short)0x0008)
#define TIM_OCPreload_Disable              ((unsigned short)0x0000)

#define TIM_IT_Update                      ((unsigned short)0x0001)
#define TIM_IT_CC1                         ((unsigned short)0x0002)
#define TIM_IT_CC2                         ((unsigned short)0x0004)
#define TIM_IT_CC3                         ((unsigned short)0x0008)
#define TIM_IT_CC4                         ((unsigned short)0x0010)
#define TIM_IT_COM                         ((unsigned short)0x0020)
#define TIM_IT_Trigger                     ((unsigned short)0x0040)
#define TIM_IT_Break                       ((unsigned short)0x0080)

#define TIM_ClockDivision_1 0U
#define TIM_ClockDivision_2 0x100U
#define TIM_ClockDivision_4 0x200U

void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct);
void TIM_ARRPreloadConfig(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_ITConfig(TIM_TypeDef* TIMx, unsigned short TIM_IT, FunctionalState NewState);
void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, unsigned short TIM_IT);
ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, unsigned short TIM_IT);
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);
void TIM_OC1_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_OC3_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_OC4_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);

void TIM_OC1PreloadConfig(TIM_TypeDef* TIMx, unsigned short TIM_OCPreload);
void TIM_OC2PreloadConfig(TIM_TypeDef* TIMx, unsigned short TIM_OCPreload);
void TIM_OC3PreloadConfig(TIM_TypeDef* TIMx, unsigned short TIM_OCPreload);
void TIM_OC4PreloadConfig(TIM_TypeDef* TIMx, unsigned short TIM_OCPreload);

void TIM_SetCompare1(TIM_TypeDef* TIMx, unsigned int Compare1);
void TIM_SetCompare2(TIM_TypeDef* TIMx, unsigned int Compare2);
void TIM_SetCompare3(TIM_TypeDef* TIMx, unsigned int Compare3);
void TIM_SetCompare4(TIM_TypeDef* TIMx, unsigned int Compare4);

#endif /*_TIM_H */
