#include <board.h>
#include <exti.h>

/**
  * @brief  Initializes the EXTI peripheral according to the specified
  *         parameters in the EXTI_InitStruct.
  * @param  EXTI_InitStruct: pointer to a EXTI_InitTypeDef structure
  *         that contains the configuration information for the EXTI peripheral.
  * @retval None
  */
void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct)
{
  if (EXTI_InitStruct->EXTI_LineCmd != DISABLE)
  {
    if (EXTI_InitStruct->EXTI_Mode == EXTI_Mode_Interrupt)
    {
      EXTI->IMR1 |= EXTI_InitStruct->EXTI_Line;
      EXTI->EMR1 &= ~EXTI_InitStruct->EXTI_Line;
    }
    else
    {
      EXTI->IMR1 &= ~EXTI_InitStruct->EXTI_Line;
      EXTI->EMR1 |= EXTI_InitStruct->EXTI_Line;
    }

    switch (EXTI_InitStruct->EXTI_Trigger)
    {
      case EXTI_Trigger_Rising:
        EXTI->RTSR1 |= EXTI_InitStruct->EXTI_Line;
        EXTI->FTSR1 &= ~EXTI_InitStruct->EXTI_Line;
        break;
      case EXTI_Trigger_Falling:
        EXTI->RTSR1 &= ~EXTI_InitStruct->EXTI_Line;
        EXTI->FTSR1 |= EXTI_InitStruct->EXTI_Line;
        break;
      case EXTI_Trigger_Rising_Falling:
        EXTI->RTSR1 |= EXTI_InitStruct->EXTI_Line;
        EXTI->FTSR1 |= EXTI_InitStruct->EXTI_Line;
        break;
    }
  }
  else
  {
    EXTI->IMR1 &= ~EXTI_InitStruct->EXTI_Line;
    EXTI->EMR1 &= ~EXTI_InitStruct->EXTI_Line;
  }
}

/**
  * @brief  Clears the EXTI's line pending bits.
  * @param  EXTI_Line: specifies the EXTI lines to clear.
  *          This parameter can be any combination of EXTI_Linex where x can be (0..22)
  * @retval None
  */
void EXTI_ClearITPendingBit(uint32_t EXTI_Line)
{
  EXTI->RPR1 = EXTI_Line;
  EXTI->FPR1 = EXTI_Line;
}

/**
  * @brief  Selects the GPIO pin used as EXTI Line.
  * @param  EXTI_PortSourceGPIOx : selects the GPIO port to be used as source for
  *          EXTI lines
  *            
  * @param  EXTI_PinSourcex: specifies the EXTI line to be configured.
  *
  * @retval None
  */
void EXTI_LineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex)
{
  uint32_t tmp = 0x00;

  tmp = ((uint32_t)0x0F) << (0x04 * (EXTI_PinSourcex & (uint8_t)0x03));
  EXTI->EXTICR[EXTI_PinSourcex >> 0x02] &= ~tmp;
  EXTI->EXTICR[EXTI_PinSourcex >> 0x02] |= (((uint32_t)EXTI_PortSourceGPIOx) << (0x04 * (EXTI_PinSourcex & (uint8_t)0x03)));
}
