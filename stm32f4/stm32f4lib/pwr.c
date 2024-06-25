#include "board.h"
#include "pwr.h"

/* Private define ------------------------------------------------------------*/
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

/* Alias word address of FPDS bit */
#define FPDS_BitNumber           0x09
#define CR_FPDS_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (FPDS_BitNumber * 4))

/* Alias word address of PMODE bit */
#define PMODE_BitNumber           0x0E
#define CR_PMODE_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PMODE_BitNumber * 4))

/* Alias word address of ODEN bit */
#define ODEN_BitNumber           0x10
#define CR_ODEN_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (ODEN_BitNumber * 4))

/* Alias word address of ODSWEN bit */
#define ODSWEN_BitNumber         0x11
#define CR_ODSWEN_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (ODSWEN_BitNumber * 4))

#if defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F446xx)
/* Alias word address of MRUDS bit */
#define MRUDS_BitNumber         0x0B
#define CR_MRUDS_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (MRUDS_BitNumber * 4))

/* Alias word address of LPUDS bit */
#define LPUDS_BitNumber         0x0A
#define CR_LPUDS_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (LPUDS_BitNumber * 4))
#endif /* STM32F427_437xx || STM32F429_439xx || STM32F446xx */

#if defined(STM32F401xx) || defined(STM32F410xx) || defined(STM32F411xE) || defined(STM32F412xG)
/* Alias word address of MRLVDS bit */
#define MRLVDS_BitNumber         0x0B
#define CR_MRLVDS_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (MRLVDS_BitNumber * 4))

/* Alias word address of LPLVDS bit */
#define LPLVDS_BitNumber         0x0A
#define CR_LPLVDS_BB             (PERIPH_BB_BASE + (CR_OFFSET * 32) + (LPLVDS_BitNumber * 4))
#endif /* STM32F401xx || STM32F410xx || STM32F411xE || STM32F412xG  */

/* --- CSR Register ---*/
#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F401xx) || defined(STM32F410xx) || defined(STM32F411xE) || defined(STM32F469_479xx)
/* Alias word address of EWUP bit */
#define CSR_OFFSET               (PWR_OFFSET + 0x04)
#define EWUP_BitNumber           0x08
#define CSR_EWUP_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (EWUP_BitNumber * 4))
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429_439xx || STM32F401xx || STM32F410xx || STM32F411xE || STM32F469_479xx */

#if defined(STM32F410xx) || defined(STM32F412xG) || defined(STM32F446xx)
/* Alias word address of EWUP2 bit */
#define CSR_OFFSET               (PWR_OFFSET + 0x04)
#define EWUP1_BitNumber           0x08
#define CSR_EWUP1_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (EWUP1_BitNumber * 4))
#define EWUP2_BitNumber           0x07
#define CSR_EWUP2_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (EWUP2_BitNumber * 4))
#if defined(STM32F410xx) || defined(STM32F412xG)
#define EWUP3_BitNumber           0x06
#define CSR_EWUP3_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (EWUP2_BitNumber * 4))
#endif /* STM32F410xx || STM32F412xG */
#endif /* STM32F410xx || STM32F412xG || STM32F446xx */

/* Alias word address of BRE bit */
#define BRE_BitNumber            0x09
#define CSR_BRE_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (BRE_BitNumber * 4))

/* ------------------ PWR registers bit mask ------------------------ */

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFF3FC)
#define CR_PLS_MASK              ((uint32_t)0xFFFFFF1F)
#define CR_VOS_MASK              ((uint32_t)0xFFFF3FFF)

/**
  * @brief  Enables or disables access to the backup domain (RTC registers, RTC 
  *         backup data registers and backup SRAM).
  * @note   If the HSE divided by 2, 3, ..31 is used as the RTC clock, the 
  *         Backup Domain Access should be kept enabled.
  * @param  NewState: new state of the access to the backup domain.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_BackupAccessCmd(FunctionalState NewState)
{
  *(__IO uint32_t *) CR_DBP_BB = (uint32_t)NewState;
}

/**
  * @brief  Enables or disables the Backup Regulator.
  * @param  NewState: new state of the Backup Regulator.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_BackupRegulatorCmd(FunctionalState NewState)
{
  *(__IO uint32_t *) CSR_BRE_BB = (uint32_t)NewState;
}

/**
  * @brief  Checks whether the specified PWR flag is set or not.
  * @param  PWR_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg PWR_FLAG_WU: Wake Up flag. This flag indicates that a wakeup event 
  *                  was received from the WKUP pin or from the RTC alarm (Alarm A 
  *                  or Alarm B), RTC Tamper event, RTC TimeStamp event or RTC Wakeup.
  *                  An additional wakeup event is detected if the WKUP pin is enabled 
  *                  (by setting the EWUP bit) when the WKUP pin level is already high.  
  *            @arg PWR_FLAG_SB: StandBy flag. This flag indicates that the system was
  *                  resumed from StandBy mode.    
  *            @arg PWR_FLAG_PVDO: PVD Output. This flag is valid only if PVD is enabled 
  *                  by the PWR_PVDCmd() function. The PVD is stopped by Standby mode 
  *                  For this reason, this bit is equal to 0 after Standby or reset
  *                  until the PVDE bit is set.
  *            @arg PWR_FLAG_BRR: Backup regulator ready flag. This bit is not reset 
  *                  when the device wakes up from Standby mode or by a system reset 
  *                  or power reset.  
  *            @arg PWR_FLAG_VOSRDY: This flag indicates that the Regulator voltage 
  *                 scaling output selection is ready.
  *            @arg PWR_FLAG_ODRDY: This flag indicates that the Over-drive mode
  *                 is ready (STM32F42xxx/43xxx devices) 
  *            @arg PWR_FLAG_ODSWRDY: This flag indicates that the Over-drive mode
  *                 switching is ready (STM32F42xxx/43xxx devices) 
  *            @arg PWR_FLAG_UDRDY: This flag indicates that the Under-drive mode
  *                 is enabled in Stop mode (STM32F42xxx/43xxx devices)
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
