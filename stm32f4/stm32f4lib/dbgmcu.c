#include "board.h"
#include "dbgmcu.h"

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
void DBGMCU_Config(unsigned int DBGMCU_Periph, FunctionalState NewState)
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

/*void DBGMCU_APB11PeriphConfig(unsigned int DBGMCU_Periph, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    DBGMCU->APB1FZR1 |= DBGMCU_Periph;
  }
  else
  {
    DBGMCU->APB1FZR1 &= ~DBGMCU_Periph;
  }
}

void DBGMCU_APB12PeriphConfig(unsigned int DBGMCU_Periph, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    DBGMCU->APB1FZR2 |= DBGMCU_Periph;
  }
  else
  {
    DBGMCU->APB1FZR2 &= ~DBGMCU_Periph;
  }
}*/

void DBGMCU_APB2PeriphConfig(unsigned int DBGMCU_Periph, FunctionalState NewState)
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
