#include "board.h"
#include "dbgmcu.h"

/* Private define ------------------------------------------------------------*/
#define IDCODE_DEVID_MASK    ((uint32_t)0x00000FFF)

/**
  * @brief  Returns the device revision identifier.
  * @param  None
  * @retval Device revision identifier
  */
uint32_t DBGMCU_GetREVID(void)
{
   return(DBGMCU->IDCODE >> 16);
}

/**
  * @brief  Returns the device identifier.
  * @param  None
  * @retval Device identifier
  */
uint32_t DBGMCU_GetDEVID(void)
{
   return(DBGMCU->IDCODE & IDCODE_DEVID_MASK);
}

/**
  * @brief  Configures low power mode behavior when the MCU is in Debug mode.
  * @param  DBGMCU_Periph: specifies the low power mode.
  *   This parameter can be any combination of the following values:
  *     @arg DBGMCU_SLEEP: Keep debugger connection during SLEEP mode
  *     @arg DBGMCU_STOP: Keep debugger connection during STOP mode
  *     @arg DBGMCU_STANDBY: Keep debugger connection during STANDBY mode
  * @param  NewState: new state of the specified low power mode in Debug mode.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DBGMCU_Config(uint32_t DBGMCU_Periph, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    DBGMCU->CR |= DBGMCU_Periph;
  }
  else
  {
    DBGMCU->CR &= ~DBGMCU_Periph;
  }
}


/**
  * @brief  Configures APB1 peripheral behavior when the MCU is in Debug mode.
  * @param  DBGMCU_Periph: specifies the APB1 peripheral.
  *   This parameter can be any combination of the following values:
  *     @arg DBGMCU_TIM2_STOP: TIM2 counter stopped when Core is halted
  *     @arg DBGMCU_TIM3_STOP: TIM3 counter stopped when Core is halted
  *     @arg DBGMCU_TIM4_STOP: TIM4 counter stopped when Core is halted
  *     @arg DBGMCU_TIM5_STOP: TIM5 counter stopped when Core is halted  
  *     @arg DBGMCU_TIM6_STOP: TIM6 counter stopped when Core is halted
  *     @arg DBGMCU_TIM7_STOP: TIM7 counter stopped when Core is halted
  *     @arg DBGMCU_RTC_STOP:
  *       + On STM32L1xx Medium-density devices: RTC Wakeup counter stopped when 
  *         Core is halted.
  *       + On STM32L1xx High-density and Medium-density Plus devices: RTC Calendar 
  *         and Wakeup counter stopped when Core is halted.
  *     @arg DBGMCU_WWDG_STOP: Debug WWDG stopped when Core is halted
  *     @arg DBGMCU_IWDG_STOP: Debug IWDG stopped when Core is halted
  *     @arg DBGMCU_I2C1_SMBUS_TIMEOUT: I2C1 SMBUS timeout mode stopped when Core is 
  *                                     halted
  *     @arg DBGMCU_I2C2_SMBUS_TIMEOUT: I2C2 SMBUS timeout mode stopped when Core is 
  *                                     halted
  * @param  NewState: new state of the specified APB1 peripheral in Debug mode.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DBGMCU_APB1PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    DBGMCU->APB1FZ |= DBGMCU_Periph;
  }
  else
  {
    DBGMCU->APB1FZ &= ~DBGMCU_Periph;
  }
}

/**
  * @brief  Configures APB2 peripheral behavior when the MCU is in Debug mode.
  * @param  DBGMCU_Periph: specifies the APB2 peripheral.
  *   This parameter can be any combination of the following values:
  *     @arg DBGMCU_TIM9_STOP: TIM9 counter stopped when Core is halted
  *     @arg DBGMCU_TIM10_STOP: TIM10 counter stopped when Core is halted
  *     @arg DBGMCU_TIM11_STOP: TIM11 counter stopped when Core is halted
  * @param  NewState: new state of the specified APB2 peripheral in Debug mode.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DBGMCU_APB2PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    DBGMCU->APB2FZ |= DBGMCU_Periph;
  }
  else
  {
    DBGMCU->APB2FZ &= ~DBGMCU_Periph;
  }
}
