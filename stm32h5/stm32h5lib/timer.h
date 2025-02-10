#ifndef _TIMER_H
#define _TIMER_H

#include <board.h>

#define TIM_IT_UPDATE           (TIM_DIER_UIE)
#define TIM_IT_CC1              (TIM_DIER_CC1IE)
#define TIM_IT_CC2              (TIM_DIER_CC2IE)
#define TIM_IT_CC3              (TIM_DIER_CC3IE)
#define TIM_IT_CC4              (TIM_DIER_CC4IE)
#define TIM_IT_COM              (TIM_DIER_COMIE)
#define TIM_IT_TRIGGER          (TIM_DIER_TIE)
#define TIM_IT_BREAK            (TIM_DIER_BIE)

#define TIM_COUNTERMODE_UP                 0
#define TIM_COUNTERMODE_DOWN               TIM_CR1_DIR
#define TIM_COUNTERMODE_CENTERALIGNED1     TIM_CR1_CMS_0
#define TIM_COUNTERMODE_CENTERALIGNED2     TIM_CR1_CMS_1
#define TIM_COUNTERMODE_CENTERALIGNED3     TIM_CR1_CMS

#define TIM_CLOCKDIVISION_DIV1                       0
#define TIM_CLOCKDIVISION_DIV2                       (TIM_CR1_CKD_0)
#define TIM_CLOCKDIVISION_DIV4                       (TIM_CR1_CKD_1)

#define TIM_ENABLE(__TIM__)                 ((__TIM__)->CR1|=TIM_CR1_CEN)
#define TIM_DISABLE(__TIM__)                 ((__TIM__)->CR1&=~TIM_CR1_CEN)
#define TIM_ENABLE_IT(__TIM__, __INTERRUPT__)    ((__TIM__)->DIER |= (__INTERRUPT__))
#define TIM_DISABLE_IT(__TIM__, __INTERRUPT__)    ((__TIM__)->DIER &= ~(__INTERRUPT__))
#define TIM_CLEAR_IT(__TIM__, __INTERRUPT__)      ((__TIM__)->SR = ~(__INTERRUPT__))
#define TIM_GET_FLAG(__TIM__, __FLAG__)          (((__TIM__)->SR &(__FLAG__)) == (__FLAG__))

/** 
  * @brief  TIM Time base Configuration Structure definition  
  */
typedef struct
{
  unsigned int Prescaler;         /*!< Specifies the prescaler value used to divide the TIM clock.
                                   This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */

  unsigned int CounterMode;       /*!< Specifies the counter mode.
                                   This parameter can be a value of @ref TIM_Counter_Mode */

  unsigned int Period;            /*!< Specifies the period value to be loaded into the active
                                   Auto-Reload Register at the next update event.
                                   This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF.  */

  unsigned int ClockDivision;     /*!< Specifies the clock division.
                                   This parameter can be a value of @ref TIM_ClockDivision */

  unsigned int RepetitionCounter;  /*!< Specifies the repetition counter value. Each time the RCR down-counter
                                    reaches zero, an update event is generated and counting restarts
                                    from the RCR value (N).
                                    This means in PWM mode that (N+1) corresponds to:
                                        - the number of PWM periods in edge-aligned mode
                                        - the number of half PWM period in center-aligned mode
                                     This parameter must be a number between Min_Data = 0x00 and Max_Data = 0xFF. 
                                     @note This parameter is valid only for TIM1 and TIM8. */
} TIM_Base_InitTypeDef;

void TIM_Base_Init(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *Structure);

#endif
