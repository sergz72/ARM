#include "board.h"
#include "pwr.h"

/* --------- PWR registers bit address in the alias region ---------- */
#define PWR_OFFSET               (PWR_BASE - PERIPH_BASE)

/* --- CR Register ---*/

/* Alias word address of DBP bit */
#define CR_OFFSET                (PWR_OFFSET + 0x00)
#define DBP_BitNumber            0x08
#define CR_DBP_BB                (PERIPH_BB_BASE + (CR_OFFSET * 32) + (DBP_BitNumber * 4))

/* Alias word address of PVDE bit */
#define PVDE_BitNumber           0x04
#define CR_PVDE_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PVDE_BitNumber * 4))

/* ------------------ PWR registers bit mask ------------------------ */

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFFFFC)
#define CR_PLS_MASK              ((uint32_t)0xFFFFFF1F)

/**
  * @brief  Deinitializes the PWR peripheral registers to their default reset values.
  * @param  None
  * @retval None
  */
/*void PWR_DeInit(void)
{
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, DISABLE);
}*/

/**
  * @brief  Enables or disables access to the RTC and backup registers.
  * @note   If the HSE divided by 32 is used as the RTC clock, the 
  *         Backup Domain Access should be kept enabled.
  * @param  NewState: new state of the access to the RTC and backup registers.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_BackupAccessCmd(FunctionalState NewState)
{
  *(__IO uint32_t *) CR_DBP_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the voltage threshold detected by the Power Voltage Detector(PVD).
  * @param  PWR_PVDLevel: specifies the PVD detection level
  *         This parameter can be one of the following values:
  *             @arg PWR_PVDLevel_0: PVD detection level set to 2.18V
  *             @arg PWR_PVDLevel_1: PVD detection level set to 2.28V
  *             @arg PWR_PVDLevel_2: PVD detection level set to 2.38V
  *             @arg PWR_PVDLevel_3: PVD detection level set to 2.48V
  *             @arg PWR_PVDLevel_4: PVD detection level set to 2.58V
  *             @arg PWR_PVDLevel_5: PVD detection level set to 2.68V
  *             @arg PWR_PVDLevel_6: PVD detection level set to 2.78V
  *             @arg PWR_PVDLevel_7: PVD detection level set to 2.88V
  * @retval None
  */
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel)
{
  uint32_t tmpreg = 0;
  
  tmpreg = PWR->CR;
  
  /* Clear PLS[7:5] bits */
  tmpreg &= CR_PLS_MASK;
  
  /* Set PLS[7:5] bits according to PWR_PVDLevel value */
  tmpreg |= PWR_PVDLevel;
  
  /* Store the new value */
  PWR->CR = tmpreg;
}

/**
  * @brief  Enables or disables the Power Voltage Detector(PVD).
  * @param  NewState: new state of the PVD.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_PVDCmd(FunctionalState NewState)
{
  *(__IO uint32_t *) CR_PVDE_BB = (uint32_t)NewState;
}

/**
  * @brief  Enables or disables the WakeUp Pin functionality.
  * @param  PWR_WakeUpPin: specifies the WakeUpPin.
  *         This parameter can be: PWR_WakeUpPin_1, PWR_WakeUpPin_2 or PWR_WakeUpPin_3.
  * @param  NewState: new state of the WakeUp Pin functionality.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_WakeUpPinCmd(uint32_t PWR_WakeUpPin, FunctionalState NewState)
{  
  if (NewState != DISABLE)
  {
    /* Enable the EWUPx pin */
    PWR->CSR |= PWR_WakeUpPin;
  }
  else
  {
    /* Disable the EWUPx pin */
    PWR->CSR &= ~PWR_WakeUpPin;
  }
}

/**
  * @brief  Enters Sleep mode.
  * @note   In Sleep mode, all I/O pins keep the same state as in Run mode.                 
  * @param  PWR_SLEEPEntry: specifies if SLEEP mode in entered with WFI or WFE instruction.
  *         This parameter can be one of the following values:
  *             @arg PWR_SLEEPEntry_WFI: enter SLEEP mode with WFI instruction
  *             @arg PWR_SLEEPEntry_WFE: enter SLEEP mode with WFE instruction
  * @retval None
  */
void PWR_EnterSleepMode(uint8_t PWR_SLEEPEntry)
{
  /* Clear SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
  
  /* Select SLEEP mode entry -------------------------------------------------*/
  if(PWR_SLEEPEntry == PWR_SLEEPEntry_WFI)
  {   
    /* Request Wait For Interrupt */
    __WFI();
  }
  else
  {
    /* Request Wait For Event */
    __SEV();
    __WFE(); 
    __WFE();
  }
}

/**
  * @brief  Enters STOP mode.
  * @note   In Stop mode, all I/O pins keep the same state as in Run mode.
  * @note   When exiting Stop mode by issuing an interrupt or a wakeup event, 
  *         the HSI RC oscillator is selected as system clock.
  * @note   When the voltage regulator operates in low power mode, an additional 
  *         startup delay is incurred when waking up from Stop mode. 
  *         By keeping the internal regulator ON during Stop mode, the consumption 
  *         is higher although the startup time is reduced.
  * @param  PWR_Regulator: specifies the regulator state in STOP mode.
  *         This parameter can be one of the following values:
  *             @arg PWR_Regulator_ON: STOP mode with regulator ON
  *             @arg PWR_Regulator_LowPower: STOP mode with regulator in low power mode
  * @param  PWR_STOPEntry: specifies if STOP mode in entered with WFI or WFE instruction.
  *         This parameter can be one of the following values:
  *             @arg PWR_STOPEntry_WFI: enter STOP mode with WFI instruction
  *             @arg PWR_STOPEntry_WFE: enter STOP mode with WFE instruction
  * @retval None
  */
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry)
{
  uint32_t tmpreg = 0;
  
  /* Select the regulator state in STOP mode ---------------------------------*/
  tmpreg = PWR->CR;
  /* Clear PDDS and LPDSR bits */
  tmpreg &= CR_DS_MASK;
  
  /* Set LPDSR bit according to PWR_Regulator value */
  tmpreg |= PWR_Regulator;
  
  /* Store the new value */
  PWR->CR = tmpreg;
  
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  
  /* Select STOP mode entry --------------------------------------------------*/
  if(PWR_STOPEntry == PWR_STOPEntry_WFI)
  {   
    /* Request Wait For Interrupt */
    __WFI();
  }
  else
  {
    /* Request Wait For Event */
    __WFE();
  }
  /* Reset SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);  
}

/**
  * @brief  Enters STANDBY mode.
  * @note   In Standby mode, all I/O pins are high impedance except for:
  * @note     Reset pad (still available) 
  * @note     RTC_AF1 pin (PC13) if configured for Wakeup pin 2 (WKUP2), tamper, 
  *           time-stamp, RTC Alarm out, or RTC clock calibration out.
  * @note     WKUP pin 1 (PA0) and WKUP pin 3 (PE6), if enabled.    
  * @note The Wakeup flag (WUF) need to be cleared at application level before to call this function.        
  * @param  None
  * @retval None
  */
void PWR_EnterSTANDBYMode(void)
{
  /* Select STANDBY mode */
  PWR->CR |= PWR_CR_PDDS;
  
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  
/* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM   )
  __force_stores();
#endif
  /* Request Wait For Interrupt */
  __WFI();
}

/**
  * @brief  Checks whether the specified PWR flag is set or not.
  * @param  PWR_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg PWR_FLAG_WU: Wake Up flag. This flag indicates that a wakeup event 
  *       was received from the WKUP pin or from the RTC alarm (Alarm A or Alarm B), 
  *       RTC Tamper event, RTC TimeStamp event or RTC Wakeup.
  *     @arg PWR_FLAG_SB: StandBy flag. This flag indicates that the system was
  *                       resumed from StandBy mode.    
  *     @arg PWR_FLAG_PVDO: PVD Output. This flag is valid only if PVD is enabled 
  *       by the PWR_PVDCmd() function.
  *     @arg PWR_FLAG_VREFINTRDY: Internal Voltage Reference Ready flag. This 
  *       flag indicates the state of the internal voltage reference, VREFINT.
  * @retval The new state of PWR_FLAG (SET or RESET).
  */
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG)
{
  FlagStatus bitstatus = RESET;
  
  if ((PWR->CSR & PWR_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the flag status */
  return bitstatus;
}

/**
  * @brief  Clears the PWR's pending flags.
  * @param  PWR_FLAG: specifies the flag to clear.
  *   This parameter can be one of the following values:
  *     @arg PWR_FLAG_WU: Wake Up flag
  *     @arg PWR_FLAG_SB: StandBy flag
  * @retval None
  */
void PWR_ClearFlag(uint32_t PWR_FLAG)
{
  PWR->CR |=  PWR_FLAG << 2;
}
