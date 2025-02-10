#include <timer.h>

/**
  * @brief  Time Base configuration
  * @param  TIMx: TIM peripheral
  * @param  Structure: pointer on TIM Time Base required parameters  
  * @retval None
  */
void TIM_Base_Init(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *Structure)
{
  unsigned int tmpcr1;
  unsigned int isCC3 = IS_TIM_CC3_INSTANCE(TIMx);
  unsigned int isCC1 = IS_TIM_CC1_INSTANCE(TIMx);
  /* Set TIM Time Base Unit parameters ---------------------------------------*/
  if (isCC3 || isCC1)
  {
    tmpcr1 = TIMx->CR1;
    if (isCC3)
    {
      /* Select the Counter Mode */
      tmpcr1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
      tmpcr1 |= Structure->CounterMode;
    }
    if(isCC1)
    {
      /* Set the clock division */
      tmpcr1 &= ~TIM_CR1_CKD;
      tmpcr1 |= (uint32_t)Structure->ClockDivision;
    }
    TIMx->CR1 = tmpcr1;
  }

  /* Set the Auto-reload value */
  TIMx->ARR = Structure->Period;
 
  /* Set the Prescaler value */
  TIMx->PSC = Structure->Prescaler;
    
  if(IS_TIM_ADVANCED_INSTANCE(TIMx))
  {
    /* Set the Repetition Counter value */
    TIMx->RCR = Structure->RepetitionCounter;
  }

  /* Generate an update event to reload the Prescaler 
     and the repetition counter(only for TIM1 and TIM8) value immediately */
  TIMx->EGR = TIM_EGR_UG;
}
