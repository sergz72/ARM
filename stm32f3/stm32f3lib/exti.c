#include "board.h"
#include "exti.h"

#define EXTI_LINENONE     ((uint32_t)0x00000)        /* No interrupt selected */

/**
  * @brief  Deinitializes the EXTI peripheral registers to their default reset 
  *         values.
  * @param  None
  * @retval None
  */
void EXTI_DeInit(void)
{
  EXTI->IMR    = 0x1F800000;
  EXTI->EMR    = 0x00000000;
  EXTI->RTSR   = 0x00000000;
  EXTI->FTSR   = 0x00000000;
  EXTI->SWIER  = 0x00000000;
  EXTI->PR     = 0xE07FFFFF;
  EXTI->IMR2   = 0x0000000C;
  EXTI->EMR2   = 0x00000000;
  EXTI->RTSR2  = 0x00000000;
  EXTI->FTSR2  = 0x00000000;
  EXTI->SWIER2 = 0x00000000;
  EXTI->PR2    = 0x00000003;
}

/**
  * @brief  Initializes the EXTI peripheral according to the specified
  *         parameters in the EXTI_InitStruct.
  *    EXTI_Line specifies the EXTI line (EXTI0....EXTI35).
  *    EXTI_Mode specifies which EXTI line is used as interrupt or an event.
  *    EXTI_Trigger selects the trigger. When the trigger occurs, interrupt
  *                 pending bit will be set.
  *    EXTI_LineCmd controls (Enable/Disable) the EXTI line.
  * @param  EXTI_InitStruct: pointer to a EXTI_InitTypeDef structure that 
  *         contains the configuration information for the EXTI peripheral.
  * @retval None
  */
void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct)
{
  uint32_t tmp = 0;

  tmp = (uint32_t)EXTI_BASE;
      
  if (EXTI_InitStruct->EXTI_LineCmd != DISABLE)
  {
    /* Clear EXTI line configuration */   
    *(__IO uint32_t *) (((uint32_t) &(EXTI->IMR)) + ((EXTI_InitStruct->EXTI_Line) >> 5 ) * 0x20) &= ~(uint32_t)(1 << (EXTI_InitStruct->EXTI_Line & 0x1F));   
    *(__IO uint32_t *) (((uint32_t) &(EXTI->EMR)) + ((EXTI_InitStruct->EXTI_Line) >> 5 ) * 0x20) &= ~(uint32_t)(1 << (EXTI_InitStruct->EXTI_Line & 0x1F));
     
    tmp += EXTI_InitStruct->EXTI_Mode + (((EXTI_InitStruct->EXTI_Line) >> 5 ) * 0x20);

    *(__IO uint32_t *) tmp |= (uint32_t)(1 << (EXTI_InitStruct->EXTI_Line & 0x1F));
    
    tmp = (uint32_t)EXTI_BASE;

    /* Clear Rising Falling edge configuration */
    *(__IO uint32_t *) (((uint32_t) &(EXTI->RTSR)) + ((EXTI_InitStruct->EXTI_Line) >> 5 ) * 0x20) &= ~(uint32_t)(1 << (EXTI_InitStruct->EXTI_Line & 0x1F));
    *(__IO uint32_t *) (((uint32_t) &(EXTI->FTSR)) + ((EXTI_InitStruct->EXTI_Line) >> 5 ) * 0x20) &= ~(uint32_t)(1 << (EXTI_InitStruct->EXTI_Line & 0x1F));
    
      /* Select the trigger for the selected interrupts */
    if (EXTI_InitStruct->EXTI_Trigger == EXTI_Trigger_Rising_Falling)
    {
      /* Rising Falling edge */
    *(__IO uint32_t *) (((uint32_t) &(EXTI->RTSR)) + ((EXTI_InitStruct->EXTI_Line) >> 5 ) * 0x20) |= (uint32_t)(1 << (EXTI_InitStruct->EXTI_Line & 0x1F));
    *(__IO uint32_t *) (((uint32_t) &(EXTI->FTSR)) + ((EXTI_InitStruct->EXTI_Line) >> 5 ) * 0x20) |= (uint32_t)(1 << (EXTI_InitStruct->EXTI_Line & 0x1F));      
    }
    else
    {
      tmp += EXTI_InitStruct->EXTI_Trigger + (((EXTI_InitStruct->EXTI_Line) >> 5 ) * 0x20);

      *(__IO uint32_t *) tmp |= (uint32_t)(1 << (EXTI_InitStruct->EXTI_Line & 0x1F));
    }
  }
      
  else
  {
    tmp += EXTI_InitStruct->EXTI_Mode + (((EXTI_InitStruct->EXTI_Line) >> 5 ) * 0x20);

    /* Disable the selected external lines */
    *(__IO uint32_t *) tmp &= ~(uint32_t)(1 << (EXTI_InitStruct->EXTI_Line & 0x1F));
  }
         
}

/**
  * @brief  Fills each EXTI_InitStruct member with its reset value.
  * @param  EXTI_InitStruct: pointer to a EXTI_InitTypeDef structure which will
  *         be initialized.
  * @retval None
  */
void EXTI_StructInit(EXTI_InitTypeDef* EXTI_InitStruct)
{
  EXTI_InitStruct->EXTI_Line = EXTI_LINENONE;
  EXTI_InitStruct->EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct->EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStruct->EXTI_LineCmd = DISABLE;
}

/**
  * @brief  Generates a Software interrupt on selected EXTI line.
  * @param  EXTI_Line: specifies the EXTI line on which the software interrupt
  *         will be generated.
  *   This parameter can be any combination of EXTI_Linex where x can be (0..20).
  * @retval None
  */
void EXTI_GenerateSWInterrupt(uint32_t EXTI_Line)
{
  *(__IO uint32_t *) (((uint32_t) &(EXTI->SWIER)) + ((EXTI_Line) >> 5 ) * 0x20) |= (uint32_t)(1 << (EXTI_Line & 0x1F));
}

/**
  * @brief  Checks whether the specified EXTI line flag is set or not.
  * @param  EXTI_Line: specifies the EXTI line flag to check.
  *   This parameter can be any combination of EXTI_Linex where x can be (0..20).
  * @retval The new state of EXTI_Line (SET or RESET).                  
  */
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line)
{
  FlagStatus bitstatus = RESET;
  
  if ((*(__IO uint32_t *) (((uint32_t) &(EXTI->PR)) + ((EXTI_Line) >> 5 ) * 0x20)& (uint32_t)(1 << (EXTI_Line & 0x1F))) != (uint32_t)RESET)
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
  * @brief  Clears the EXTI's line pending flags.
  * @param  EXTI_Line: specifies the EXTI lines flags to clear.
  *   This parameter can be any combination of EXTI_Linex where x can be (0..20).
  * @retval None
  */
void EXTI_ClearFlag(uint32_t EXTI_Line)
{
  *(__IO uint32_t *) (((uint32_t) &(EXTI->PR)) + ((EXTI_Line) >> 5 ) * 0x20) = (1 << (EXTI_Line & 0x1F));  
}

/**
  * @brief  Checks whether the specified EXTI line is asserted or not.
  * @param  EXTI_Line: specifies the EXTI line to check.
  *   This parameter can be any combination of EXTI_Linex where x can be (0..20).
  * @retval The new state of EXTI_Line (SET or RESET).
  */
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line)
{
  ITStatus bitstatus = RESET;
  
  if ((*(__IO uint32_t *) (((uint32_t) &(EXTI->PR)) + ((EXTI_Line) >> 5 ) * 0x20)& (uint32_t)(1 << (EXTI_Line & 0x1F))) != (uint32_t)RESET)
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
  * @brief  Clears the EXTI's line pending bits.
  * @param  EXTI_Line: specifies the EXTI lines to clear.
  *   This parameter can be any combination of EXTI_Linex where x can be (0..20).
  * @retval None
  */
void EXTI_ClearITPendingBit(uint32_t EXTI_Line)
{
  *(__IO uint32_t *) (((uint32_t) &(EXTI->PR)) + ((EXTI_Line) >> 5 ) * 0x20) = (1 << (EXTI_Line & 0x1F));
}
