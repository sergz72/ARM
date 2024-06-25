#include "timer.h"

#define TIM_CHANNEL_1                      0x00000000U                          /*!< Capture/compare channel 1 identifier      */
#define TIM_CHANNEL_2                      0x00000004U                          /*!< Capture/compare channel 2 identifier      */
#define TIM_CHANNEL_3                      0x00000008U                          /*!< Capture/compare channel 3 identifier      */
#define TIM_CHANNEL_4                      0x0000000CU                          /*!< Capture/compare channel 4 identifier      */

/**
  * @brief  Checks whether the TIM interrupt has occurred or not.
  * @param  TIMx: where x can be 1, 2, 3, 6, 7, 14, 15, 16 or 17 to select the TIM peripheral.
  * @note   TIM7 is applicable only for STM32F072 devices
  * @note   TIM6 is not applivable for STM32F031 devices.
  * @note   TIM2 is not applicable for STM32F030 devices.
  * @param  TIM_IT: specifies the TIM interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg TIM_IT_Update: TIM update Interrupt source
  *            @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *            @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *            @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *            @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *            @arg TIM_IT_COM: TIM Commutation Interrupt source
  *            @arg TIM_IT_Trigger: TIM Trigger Interrupt source
  *            @arg TIM_IT_Break: TIM Break Interrupt source
  *
  * @note   TIM6 and TIM7 can generate only an update interrupt.
  * @note   TIM15 can have only TIM_IT_Update, TIM_IT_CC1, TIM_IT_CC2 or TIM_IT_Trigger. 
  * @note   TIM14, TIM16 and TIM17 can have TIM_IT_Update or TIM_IT_CC1.   
  * @note   TIM_IT_Break is used only with TIM1 and TIM15. 
  * @note   TIM_IT_COM is used only with TIM1, TIM15, TIM16 and TIM17.
  *
  * @retval The new state of the TIM_IT(SET or RESET).
  */
ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT)
{
  ITStatus bitstatus = RESET;  
  uint16_t itstatus = 0x0, itenable = 0x0;
   
  itstatus = TIMx->SR & TIM_IT;
  
  itenable = TIMx->DIER & TIM_IT;
  if ((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

/**
  * @brief  Clears the TIMx's interrupt pending bits.
  * @param  TIMx: where x can be 1, 2, 3, 6, 7, 14, 15, 16 or 17 to select the TIM peripheral.
  * @note   TIM7 is applicable only for STM32F072 devices
  * @note   TIM6 is not applivable for STM32F031 devices.
  * @note   TIM2 is not applicable for STM32F030 devices.
  * @param  TIM_IT: specifies the pending bit to clear.
  *          This parameter can be any combination of the following values:
  *            @arg TIM_IT_Update: TIM1 update Interrupt source
  *            @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *            @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *            @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *            @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *            @arg TIM_IT_COM: TIM Commutation Interrupt source
  *            @arg TIM_IT_Trigger: TIM Trigger Interrupt source
  *            @arg TIM_IT_Break: TIM Break Interrupt source
  *
  * @note   TIM6 and TIM7 can generate only an update interrupt.
  * @note   TIM15 can have only TIM_IT_Update, TIM_IT_CC1, TIM_IT_CC2 or TIM_IT_Trigger. 
  * @note   TIM14, TIM16 and TIM17 can have TIM_IT_Update or TIM_IT_CC1.   
  * @note   TIM_IT_Break is used only with TIM1 and TIM15. 
  * @note   TIM_IT_COM is used only with TIM1, TIM15, TIM16 and TIM17.
  *
  * @retval None
  */
void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT)
{
  /* Clear the IT pending Bit */
  TIMx->SR = (uint16_t)~TIM_IT;
}

/**
  * @brief  Initializes the TIMx Time Base Unit peripheral according to 
  *         the specified parameters in the TIM_TimeBaseInitStruct.
  * @param  TIMx: where x can be 1, 2, 3, 6, 7, 14, 15, 16 and 17 to select the TIM 
  *         peripheral.
  * @note   TIM7 is applicable only for STM32F072 devices
  * @note   TIM6 is not applivable for STM32F031 devices.
  * @note   TIM2 is not applicable for STM32F030 devices.  
  * @param  TIM_TimeBaseInitStruct: pointer to a TIM_TimeBaseInitTypeDef
  *         structure that contains the configuration information for
  *         the specified TIM peripheral.
  * @retval None
  */
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct)
{
  uint16_t tmpcr1 = 0;

  tmpcr1 = TIMx->CR1;  

  if((TIMx == TIM1) || (TIMx == TIM2) || (TIMx == TIM3))
  {
    /* Select the Counter Mode */
    tmpcr1 &= (uint16_t)(~((uint16_t)(TIM_CR1_DIR | TIM_CR1_CMS)));
    tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_CounterMode;
  }
 
  if(TIMx != TIM6)
  {
    /* Set the clock division */
    tmpcr1 &= (uint16_t)(~((uint16_t)TIM_CR1_CKD));
    tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_ClockDivision;
  }

  TIMx->CR1 = tmpcr1;

  /* Set the Autoreload value */
  TIMx->ARR = TIM_TimeBaseInitStruct->TIM_Period ;
 
  /* Set the Prescaler value */
  TIMx->PSC = TIM_TimeBaseInitStruct->TIM_Prescaler;
    
  if ((TIMx == TIM1) || (TIMx == TIM15)|| (TIMx == TIM16) || (TIMx == TIM17))  
  {
    /* Set the Repetition Counter value */
    TIMx->RCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;
  }

  /* Generate an update event to reload the Prescaler and the Repetition counter
     values immediately */
  TIMx->EGR = TIM_PSCReloadMode_Immediate;           
}

/**
  * @brief  Enables or disables TIMx peripheral Preload register on ARR.
  * @param  TIMx: where x can be  1, 2, 3, 6, 7, 14, 15, 16 and 17 to select the TIM 
  *         peripheral.
  * @note   TIM7 is applicable only for STM32F072 devices
  * @note   TIM6 is not applivable for STM32F031 devices.
  * @note   TIM2 is not applicable for STM32F030 devices.  
  * @param  NewState: new state of the TIMx peripheral Preload register
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_ARRPreloadConfig(TIM_TypeDef* TIMx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Set the ARR Preload Bit */
    TIMx->CR1 |= TIM_CR1_ARPE;
  }
  else
  {
    /* Reset the ARR Preload Bit */
    TIMx->CR1 &= (uint16_t)~((uint16_t)TIM_CR1_ARPE);
  }
}

/**
  * @brief  Enables or disables the specified TIM interrupts.
  * @param  TIMx: where x can be 1, 2, 3, 6, 7, 14, 15, 16 or 17 to select the TIMx peripheral.
  * @note   TIM7 is applicable only for STM32F072 devices
  * @note   TIM6 is not applivable for STM32F031 devices.
  * @note   TIM2 is not applicable for STM32F030 devices.
  * @param  TIM_IT: specifies the TIM interrupts sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg TIM_IT_Update: TIM update Interrupt source
  *            @arg TIM_IT_CC1: TIM Capture Compare 1 Interrupt source
  *            @arg TIM_IT_CC2: TIM Capture Compare 2 Interrupt source
  *            @arg TIM_IT_CC3: TIM Capture Compare 3 Interrupt source
  *            @arg TIM_IT_CC4: TIM Capture Compare 4 Interrupt source
  *            @arg TIM_IT_COM: TIM Commutation Interrupt source
  *            @arg TIM_IT_Trigger: TIM Trigger Interrupt source
  *            @arg TIM_IT_Break: TIM Break Interrupt source
  * 
  * @note   TIM6 and TIM7 can only generate an update interrupt.
  * @note   TIM15 can have only TIM_IT_Update, TIM_IT_CC1,TIM_IT_CC2 or TIM_IT_Trigger. 
  * @note   TIM14, TIM16 and TIM17 can have TIM_IT_Update or TIM_IT_CC1.   
  * @note   TIM_IT_Break is used only with TIM1 and TIM15. 
  * @note   TIM_IT_COM is used only with TIM1, TIM15, TIM16 and TIM17.
  *       
  * @param  NewState: new state of the TIM interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState)
{  
  if (NewState != DISABLE)
  {
    /* Enable the Interrupt sources */
    TIMx->DIER |= TIM_IT;
  }
  else
  {
    /* Disable the Interrupt sources */
    TIMx->DIER &= (uint16_t)~TIM_IT;
  }
}

/**
  * @brief  Enables or disables the specified TIM peripheral.
  * @param  TIMx: where x can be 1, 2, 3, 6, 7, 14, 15, 16 and 17to select the TIMx
  *         peripheral.
  * @note   TIM7 is applicable only for STM32F072 devices
  * @note   TIM6 is not applivable for STM32F031 devices.
  * @note   TIM2 is not applicable for STM32F030 devices.    
  * @param  NewState: new state of the TIMx peripheral.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the TIM Counter */
    TIMx->CR1 |= TIM_CR1_CEN;
  }
  else
  {
    /* Disable the TIM Counter */
    TIMx->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
  }
}

/**
  * @brief  Timer Output Compare 1 configuration
  * @param  TIMx to select the TIM peripheral
  * @param  OC_Config The ouput configuration structure
  * @retval None
  */
void TIM_OC1_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config)
{
  uint32_t tmpccmrx;
  uint32_t tmpccer;
  uint32_t tmpcr2;

  /* Disable the Channel 1: Reset the CC1E Bit */
  TIMx->CCER &= ~TIM_CCER_CC1E;

  /* Get the TIMx CCER register value */
  tmpccer = TIMx->CCER;
  /* Get the TIMx CR2 register value */
  tmpcr2 =  TIMx->CR2;

  /* Get the TIMx CCMR1 register value */
  tmpccmrx = TIMx->CCMR1;

  /* Reset the Output Compare Mode Bits */
  tmpccmrx &= ~TIM_CCMR1_OC1M;
  tmpccmrx &= ~TIM_CCMR1_CC1S;
  /* Select the Output Compare Mode */
  tmpccmrx |= OC_Config->OCMode;

  /* Reset the Output Polarity level */
  tmpccer &= ~TIM_CCER_CC1P;
  /* Set the Output Compare Polarity */
  tmpccer |= OC_Config->OCPolarity;

  if (IS_TIM_CCXN_INSTANCE(TIMx, TIM_CHANNEL_1))
  {
    /* Reset the Output N Polarity level */
    tmpccer &= ~TIM_CCER_CC1NP;
    /* Set the Output N Polarity */
    tmpccer |= OC_Config->OCNPolarity;
    /* Reset the Output N State */
    tmpccer &= ~TIM_CCER_CC1NE;

    /* Set the Output State */
    tmpccer |= OC_Config->OutputNState;
  }

  if (IS_TIM_BREAK_INSTANCE(TIMx))
  {
    /* Reset the Output Compare and Output Compare N IDLE State */
    tmpcr2 &= ~TIM_CR2_OIS1;
    tmpcr2 &= ~TIM_CR2_OIS1N;
    /* Set the Output Idle state */
    tmpcr2 |= OC_Config->OCIdleState;
    /* Set the Output N Idle state */
    tmpcr2 |= OC_Config->OCNIdleState;
  }

  /* Write to TIMx CR2 */
  TIMx->CR2 = tmpcr2;

  /* Write to TIMx CCMR1 */
  TIMx->CCMR1 = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIMx->CCR1 = OC_Config->Pulse;

  /* Set the Output State */
  tmpccer |= OC_Config->OutputState;

  /* Write to TIMx CCER */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Timer Output Compare 2 configuration
  * @param  TIMx to select the TIM peripheral
  * @param  OC_Config The ouput configuration structure
  * @retval None
  */
void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config)
{
  uint32_t tmpccmrx;
  uint32_t tmpccer;
  uint32_t tmpcr2;

  /* Disable the Channel 2: Reset the CC2E Bit */
  TIMx->CCER &= ~TIM_CCER_CC2E;

  /* Get the TIMx CCER register value */
  tmpccer = TIMx->CCER;
  /* Get the TIMx CR2 register value */
  tmpcr2 =  TIMx->CR2;

  /* Get the TIMx CCMR1 register value */
  tmpccmrx = TIMx->CCMR1;

  /* Reset the Output Compare mode and Capture/Compare selection Bits */
  tmpccmrx &= ~TIM_CCMR1_OC2M;
  tmpccmrx &= ~TIM_CCMR1_CC2S;

  /* Select the Output Compare Mode */
  tmpccmrx |= (OC_Config->OCMode << 8U);

  /* Reset the Output Polarity level */
  tmpccer &= ~TIM_CCER_CC2P;
  /* Set the Output Compare Polarity */
  tmpccer |= (OC_Config->OCPolarity << 4U);

  if (IS_TIM_CCXN_INSTANCE(TIMx, TIM_CHANNEL_2))
  {
    /* Reset the Output N Polarity level */
    tmpccer &= ~TIM_CCER_CC2NP;
    /* Set the Output N Polarity */
    tmpccer |= (OC_Config->OCNPolarity << 4U);
    /* Reset the Output N State */
    tmpccer &= ~TIM_CCER_CC2NE;

    /* Set the Output State */
    tmpccer |= OC_Config->OutputNState << 4;
  }

  if (IS_TIM_BREAK_INSTANCE(TIMx))
  {
    /* Reset the Output Compare and Output Compare N IDLE State */
    tmpcr2 &= ~TIM_CR2_OIS2;
    tmpcr2 &= ~TIM_CR2_OIS2N;
    /* Set the Output Idle state */
    tmpcr2 |= (OC_Config->OCIdleState << 2U);
    /* Set the Output N Idle state */
    tmpcr2 |= (OC_Config->OCNIdleState << 2U);
  }

  /* Write to TIMx CR2 */
  TIMx->CR2 = tmpcr2;

  /* Write to TIMx CCMR1 */
  TIMx->CCMR1 = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIMx->CCR2 = OC_Config->Pulse;

  /* Set the Output State */
  tmpccer |= OC_Config->OutputState << 4;

  /* Write to TIMx CCER */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Timer Output Compare 3 configuration
  * @param  TIMx to select the TIM peripheral
  * @param  OC_Config The ouput configuration structure
  * @retval None
  */
void TIM_OC3_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config)
{
  uint32_t tmpccmrx;
  uint32_t tmpccer;
  uint32_t tmpcr2;

  /* Disable the Channel 3: Reset the CC2E Bit */
  TIMx->CCER &= ~TIM_CCER_CC3E;

  /* Get the TIMx CCER register value */
  tmpccer = TIMx->CCER;
  /* Get the TIMx CR2 register value */
  tmpcr2 =  TIMx->CR2;

  /* Get the TIMx CCMR2 register value */
  tmpccmrx = TIMx->CCMR2;

  /* Reset the Output Compare mode and Capture/Compare selection Bits */
  tmpccmrx &= ~TIM_CCMR2_OC3M;
  tmpccmrx &= ~TIM_CCMR2_CC3S;
  /* Select the Output Compare Mode */
  tmpccmrx |= OC_Config->OCMode;

  /* Reset the Output Polarity level */
  tmpccer &= ~TIM_CCER_CC3P;
  /* Set the Output Compare Polarity */
  tmpccer |= (OC_Config->OCPolarity << 8U);

  if (IS_TIM_CCXN_INSTANCE(TIMx, TIM_CHANNEL_3))
  {
    /* Reset the Output N Polarity level */
    tmpccer &= ~TIM_CCER_CC3NP;
    /* Set the Output N Polarity */
    tmpccer |= (OC_Config->OCNPolarity << 8U);
    /* Reset the Output N State */
    tmpccer &= ~TIM_CCER_CC3NE;

    /* Set the Output State */
    tmpccer |= OC_Config->OutputNState << 8;
  }

  if (IS_TIM_BREAK_INSTANCE(TIMx))
  {
    /* Reset the Output Compare and Output Compare N IDLE State */
    tmpcr2 &= ~TIM_CR2_OIS3;
    tmpcr2 &= ~TIM_CR2_OIS3N;
    /* Set the Output Idle state */
    tmpcr2 |= (OC_Config->OCIdleState << 4U);
    /* Set the Output N Idle state */
    tmpcr2 |= (OC_Config->OCNIdleState << 4U);
  }

  /* Write to TIMx CR2 */
  TIMx->CR2 = tmpcr2;

  /* Write to TIMx CCMR2 */
  TIMx->CCMR2 = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIMx->CCR3 = OC_Config->Pulse;

  /* Set the Output State */
  tmpccer |= OC_Config->OutputState << 8;

  /* Write to TIMx CCER */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Timer Output Compare 4 configuration
  * @param  TIMx to select the TIM peripheral
  * @param  OC_Config The ouput configuration structure
  * @retval None
  */
void TIM_OC4_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config)
{
  uint32_t tmpccmrx;
  uint32_t tmpccer;
  uint32_t tmpcr2;

  /* Disable the Channel 4: Reset the CC4E Bit */
  TIMx->CCER &= ~TIM_CCER_CC4E;

  /* Get the TIMx CCER register value */
  tmpccer = TIMx->CCER;
  /* Get the TIMx CR2 register value */
  tmpcr2 =  TIMx->CR2;

  /* Get the TIMx CCMR2 register value */
  tmpccmrx = TIMx->CCMR2;

  /* Reset the Output Compare mode and Capture/Compare selection Bits */
  tmpccmrx &= ~TIM_CCMR2_OC4M;
  tmpccmrx &= ~TIM_CCMR2_CC4S;

  /* Select the Output Compare Mode */
  tmpccmrx |= (OC_Config->OCMode << 8U);

  /* Reset the Output Polarity level */
  tmpccer &= ~TIM_CCER_CC4P;
  /* Set the Output Compare Polarity */
  tmpccer |= (OC_Config->OCPolarity << 12U);

  if (IS_TIM_BREAK_INSTANCE(TIMx))
  {
    /* Reset the Output Compare IDLE State */
    tmpcr2 &= ~TIM_CR2_OIS4;

    /* Set the Output Idle state */
    tmpcr2 |= (OC_Config->OCIdleState << 6U);
  }

  /* Write to TIMx CR2 */
  TIMx->CR2 = tmpcr2;

  /* Write to TIMx CCMR2 */
  TIMx->CCMR2 = tmpccmrx;

  /* Set the Capture Compare Register value */
  TIMx->CCR4 = OC_Config->Pulse;

  /* Set the Output State */
  tmpccer |= OC_Config->OutputState << 12;

  /* Write to TIMx CCER */
  TIMx->CCER = tmpccer;
}

/**
  * @brief  Enables or disables the TIMx peripheral Preload register on CCR1.
  * @param  TIMx: where x can be  1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  TIM_OCPreload: new state of the TIMx peripheral Preload register
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPreload_Enable
  *     @arg TIM_OCPreload_Disable
  * @retval None
  */
void TIM_OC1PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr1 = TIMx->CCMR1;
  /* Reset the OC1PE Bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC1PE);
  /* Enable or Disable the Output Compare Preload feature */
  tmpccmr1 |= TIM_OCPreload;
  /* Write to TIMx CCMR1 register */
  TIMx->CCMR1 = tmpccmr1;
}

/**
  * @brief  Enables or disables the TIMx peripheral Preload register on CCR2.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5, 8, 9, 12 or 15 to select 
  *         the TIM peripheral.
  * @param  TIM_OCPreload: new state of the TIMx peripheral Preload register
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPreload_Enable
  *     @arg TIM_OCPreload_Disable
  * @retval None
  */
void TIM_OC2PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr1 = TIMx->CCMR1;
  /* Reset the OC2PE Bit */
  tmpccmr1 &= (uint16_t)~((uint16_t)TIM_CCMR1_OC2PE);
  /* Enable or Disable the Output Compare Preload feature */
  tmpccmr1 |= (uint16_t)(TIM_OCPreload << 8);
  /* Write to TIMx CCMR1 register */
  TIMx->CCMR1 = tmpccmr1;
}

/**
  * @brief  Enables or disables the TIMx peripheral Preload register on CCR3.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCPreload: new state of the TIMx peripheral Preload register
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPreload_Enable
  *     @arg TIM_OCPreload_Disable
  * @retval None
  */
void TIM_OC3PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr2 = TIMx->CCMR2;
  /* Reset the OC3PE Bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC3PE);
  /* Enable or Disable the Output Compare Preload feature */
  tmpccmr2 |= TIM_OCPreload;
  /* Write to TIMx CCMR2 register */
  TIMx->CCMR2 = tmpccmr2;
}

/**
  * @brief  Enables or disables the TIMx peripheral Preload register on CCR4.
  * @param  TIMx: where x can be  1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  TIM_OCPreload: new state of the TIMx peripheral Preload register
  *   This parameter can be one of the following values:
  *     @arg TIM_OCPreload_Enable
  *     @arg TIM_OCPreload_Disable
  * @retval None
  */
void TIM_OC4PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload)
{
  uint16_t tmpccmr2 = TIMx->CCMR2;
  /* Reset the OC4PE Bit */
  tmpccmr2 &= (uint16_t)~((uint16_t)TIM_CCMR2_OC4PE);
  /* Enable or Disable the Output Compare Preload feature */
  tmpccmr2 |= (uint16_t)(TIM_OCPreload << 8);
  /* Write to TIMx CCMR2 register */
  TIMx->CCMR2 = tmpccmr2;
}

/**
  * @brief  Enables or disables the TIM peripheral Main Outputs.
  * @param  TIMx: where x can be 1, 8, 15, 16 or 17 to select the TIMx peripheral.
  * @param  NewState: new state of the TIM peripheral Main Outputs.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    /* Enable the TIM Main Output */
    TIMx->BDTR |= TIM_BDTR_MOE;
  }
  else
  {
    /* Disable the TIM Main Output */
    TIMx->BDTR &= (uint16_t)(~((uint16_t)TIM_BDTR_MOE));
  }  
}

/**
  * @brief  Sets the TIMx Capture Compare1 Register value
  * @param  TIMx: where x can be 1 to 17 except 6 and 7 to select the TIM peripheral.
  * @param  Compare1: specifies the Capture Compare1 register new value.
  * @retval None
  */
void TIM_SetCompare1(TIM_TypeDef* TIMx, unsigned int Compare1)
{
  /* Set the Capture Compare1 Register value */
  TIMx->CCR1 = Compare1;
}

/**
  * @brief  Sets the TIMx Capture Compare2 Register value
  * @param  TIMx: where x can be 1, 2, 3, 4, 5, 8, 9, 12 or 15 to select the TIM peripheral.
  * @param  Compare2: specifies the Capture Compare2 register new value.
  * @retval None
  */
void TIM_SetCompare2(TIM_TypeDef* TIMx, unsigned int Compare2)
{
  /* Set the Capture Compare2 Register value */
  TIMx->CCR2 = Compare2;
}

/**
  * @brief  Sets the TIMx Capture Compare3 Register value
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  Compare3: specifies the Capture Compare3 register new value.
  * @retval None
  */
void TIM_SetCompare3(TIM_TypeDef* TIMx, unsigned int Compare3)
{
  /* Set the Capture Compare3 Register value */
  TIMx->CCR3 = Compare3;
}

/**
  * @brief  Sets the TIMx Capture Compare4 Register value
  * @param  TIMx: where x can be 1, 2, 3, 4, 5 or 8 to select the TIM peripheral.
  * @param  Compare4: specifies the Capture Compare4 register new value.
  * @retval None
  */
void TIM_SetCompare4(TIM_TypeDef* TIMx, unsigned int Compare4)
{
  /* Set the Capture Compare4 Register value */
  TIMx->CCR4 = Compare4;
}
