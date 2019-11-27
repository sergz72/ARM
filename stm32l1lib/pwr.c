#include <board.h>
#include <pwr.h>

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

/* Alias word address of ULP bit */
#define ULP_BitNumber           0x09
#define CR_ULP_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (ULP_BitNumber * 4))

/* Alias word address of FWU bit */
#define FWU_BitNumber           0x0A
#define CR_FWU_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (FWU_BitNumber * 4))

/* --- CSR Register ---*/

/* Alias word address of EWUP bit */
#define CSR_OFFSET               (PWR_OFFSET + 0x04)
#define EWUP_BitNumber           0x08
#define CSR_EWUP_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (EWUP_BitNumber * 4))

/* ------------------ PWR registers bit mask ------------------------ */

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFFFFC)
#define CR_PLS_MASK              ((uint32_t)0xFFFFFF1F)
#define CR_VOS_MASK              ((uint32_t)0xFFFFE7FF)

/**
  * @brief  Enables or disables access to the RTC and backup registers.
  * @note   If the HSE divided by 2, 4, 8 or 16 is used as the RTC clock, the 
  *         RTC Domain Access should be kept enabled.
  * @param  NewState: new state of the access to the RTC and backup registers.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PWR_RTCAccessCmd(FunctionalState NewState)
{
  *(__IO uint32_t *) CR_DBP_BB = (uint32_t)NewState;
}
