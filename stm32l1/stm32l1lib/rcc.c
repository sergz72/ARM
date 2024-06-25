#include <board.h>
#include <rcc.h>

/* ------------ RCC registers bit address in the alias region ----------- */
#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)

/* --- CR Register ---*/

/* Alias word address of HSION bit */
#define CR_OFFSET                 (RCC_OFFSET + 0x00)
#define HSION_BitNumber           0x00
#define CR_HSION_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (HSION_BitNumber * 4))

/* Alias word address of MSION bit */
#define MSION_BitNumber           0x08
#define CR_MSION_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (MSION_BitNumber * 4))

/* Alias word address of PLLON bit */
#define PLLON_BitNumber           0x18
#define CR_PLLON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLON_BitNumber * 4))

/* Alias word address of CSSON bit */
#define CSSON_BitNumber           0x1C
#define CR_CSSON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (CSSON_BitNumber * 4))

/* --- CSR Register ---*/

/* Alias word address of LSION bit */
#define CSR_OFFSET                (RCC_OFFSET + 0x34)
#define LSION_BitNumber           0x00
#define CSR_LSION_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (LSION_BitNumber * 4))

/* Alias word address of LSECSSON bit */
#define LSECSSON_BitNumber        0x0B
#define CSR_LSECSSON_BB          (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (LSECSSON_BitNumber * 4))

/* Alias word address of RTCEN bit */
#define RTCEN_BitNumber           0x16
#define CSR_RTCEN_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (RTCEN_BitNumber * 4))

/* Alias word address of RTCRST bit */
#define RTCRST_BitNumber          0x17
#define CSR_RTCRST_BB             (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (RTCRST_BitNumber * 4))


/* ---------------------- RCC registers mask -------------------------------- */
/* RCC Flag Mask */
#define FLAG_MASK                 ((uint8_t)0x1F)

/* CR register byte 3 (Bits[23:16]) base address */
#define CR_BYTE3_ADDRESS          ((uint32_t)0x40023802)

/* ICSCR register byte 4 (Bits[31:24]) base address */
#define ICSCR_BYTE4_ADDRESS       ((uint32_t)0x40023807)

/* CFGR register byte 3 (Bits[23:16]) base address */
#define CFGR_BYTE3_ADDRESS        ((uint32_t)0x4002380A)

/* CFGR register byte 4 (Bits[31:24]) base address */
#define CFGR_BYTE4_ADDRESS        ((uint32_t)0x4002380B)

/* CIR register byte 2 (Bits[15:8]) base address */
#define CIR_BYTE2_ADDRESS         ((uint32_t)0x4002380D)

/* CIR register byte 3 (Bits[23:16]) base address */
#define CIR_BYTE3_ADDRESS         ((uint32_t)0x4002380E)

/* CSR register byte 2 (Bits[15:8]) base address */
#define CSR_BYTE2_ADDRESS         ((uint32_t)0x40023835)

/**
  * @brief  Configures the External Low Speed oscillator (LSE).
  * @note     As the LSE is in the RTC domain and write access is denied to this
  *           domain after reset, you have to enable write access using 
  *           PWR_RTCAccessCmd(ENABLE) function before to configure the LSE
  *           (to be done once after reset).  
  * @note     Care must be taken when using this function to configure LSE mode 
  *           as it clears systematically the LSEON bit before any new configuration.
  * @note     After enabling the LSE (RCC_LSE_ON or RCC_LSE_Bypass), the application
  *           software should wait on LSERDY flag to be set indicating that LSE clock
  *           is stable and can be used to clock the RTC.
  * @param  RCC_LSE: specifies the new state of the LSE.
  *   This parameter can be one of the following values:
  *     @arg RCC_LSE_OFF: turn OFF the LSE oscillator, LSERDY flag goes low after
  *                       6 LSE oscillator clock cycles.
  *     @arg RCC_LSE_ON: turn ON the LSE oscillator
  *     @arg RCC_LSE_Bypass: LSE oscillator bypassed with external clock
  * @retval None
  */
void RCC_LSEConfig(uint8_t RCC_LSE)
{
  /* Reset LSEON and LSEBYP bits before configuring the LSE ------------------*/
  *(__IO uint8_t *) CSR_BYTE2_ADDRESS = RCC_LSE_OFF;

  /* Set the new LSE configuration -------------------------------------------*/
  *(__IO uint8_t *) CSR_BYTE2_ADDRESS = RCC_LSE;  
}

/**
  * @brief  Configures the RTC and LCD clock (RTCCLK / LCDCLK).
  * @note     As the RTC clock configuration bits are in the RTC domain and write
  *           access is denied to this domain after reset, you have to enable write
  *           access using PWR_RTCAccessCmd(ENABLE) function before to configure
  *           the RTC clock source (to be done once after reset).    
  * @note     Once the RTC clock is configured it can't be changed unless the RTC
  *           is reset using RCC_RTCResetCmd function, or by a Power On Reset (POR)
  * @note     The RTC clock (RTCCLK) is used also to clock the LCD (LCDCLK).
  *             
  * @param  RCC_RTCCLKSource: specifies the RTC clock source.
  *   This parameter can be one of the following values:
  *     @arg RCC_RTCCLKSource_LSE: LSE selected as RTC clock
  *     @arg RCC_RTCCLKSource_LSI: LSI selected as RTC clock
  *     @arg RCC_RTCCLKSource_HSE_Div2: HSE divided by 2 selected as RTC clock
  *     @arg RCC_RTCCLKSource_HSE_Div4: HSE divided by 4 selected as RTC clock
  *     @arg RCC_RTCCLKSource_HSE_Div8: HSE divided by 8 selected as RTC clock
  *     @arg RCC_RTCCLKSource_HSE_Div16: HSE divided by 16 selected as RTC clock
  *       
  * @note     If the LSE or LSI is used as RTC clock source, the RTC continues to
  *           work in STOP and STANDBY modes, and can be used as wakeup source.
  *           However, when the HSE clock is used as RTC clock source, the RTC
  *           cannot be used in STOP and STANDBY modes.
  *             
  * @note     The maximum input clock frequency for RTC is 1MHz (when using HSE as
  *           RTC clock source).
  *                          
  * @retval None
  */
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource)
{
  uint32_t 	tmpreg = 0;

  if ((RCC_RTCCLKSource & RCC_CSR_RTCSEL_HSE) == RCC_CSR_RTCSEL_HSE)
  { 
    /* If HSE is selected as RTC clock source, configure HSE division factor for RTC clock */
    tmpreg = RCC->CR;

    /* Clear RTCPRE[1:0] bits */
    tmpreg &= ~RCC_CR_RTCPRE;

    /* Configure HSE division factor for RTC clock */
    tmpreg |= (RCC_RTCCLKSource & RCC_CR_RTCPRE);

    /* Store the new value */
    RCC->CR = tmpreg;
  }
         
  RCC->CSR &= ~RCC_CSR_RTCSEL;
  
  /* Select the RTC clock source */
  RCC->CSR |= (RCC_RTCCLKSource & RCC_CSR_RTCSEL);
}

/**
  * @brief  Enables or disables the RTC clock.
  * @note   This function must be used only after the RTC clock source was selected
  *         using the RCC_RTCCLKConfig function.
  * @param  NewState: new state of the RTC clock.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_RTCCLKCmd(FunctionalState NewState)
{
  *(__IO uint32_t *) CSR_RTCEN_BB = (uint32_t)NewState;
}

/**
  * @brief  Forces or releases the RTC peripheral and associated resources reset.
  * @note   This function resets the RTC peripheral, RTC clock source selection
  *         (in RCC_CSR) and the backup registers.
  * @param  NewState: new state of the RTC reset.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_RTCResetCmd(FunctionalState NewState)
{
  *(__IO uint32_t *) CSR_RTCRST_BB = (uint32_t)NewState;
}

/**
  * @brief  Checks whether the specified RCC flag is set or not.
  * @param  RCC_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready
  *     @arg RCC_FLAG_MSIRDY: MSI oscillator clock ready  
  *     @arg RCC_FLAG_HSERDY: HSE oscillator clock ready
  *     @arg RCC_FLAG_PLLRDY: PLL clock ready
  *     @arg RCC_FLAG_LSECSS: LSE oscillator clock CSS detected  
  *     @arg RCC_FLAG_LSERDY: LSE oscillator clock ready
  *     @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready
  *     @arg RCC_FLAG_OBLRST: Option Byte Loader (OBL) reset 
  *     @arg RCC_FLAG_PINRST: Pin reset
  *     @arg RCC_FLAG_PORRST: POR/PDR reset
  *     @arg RCC_FLAG_SFTRST: Software reset
  *     @arg RCC_FLAG_IWDGRST: Independent Watchdog reset
  *     @arg RCC_FLAG_WWDGRST: Window Watchdog reset
  *     @arg RCC_FLAG_LPWRRST: Low Power reset
  * @retval The new state of RCC_FLAG (SET or RESET).
  */
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG)
{
  uint32_t tmp = 0;
  uint32_t statusreg = 0;
  FlagStatus bitstatus = RESET;

  /* Get the RCC register index */
  tmp = RCC_FLAG >> 5;

  if (tmp == 1)               /* The flag to check is in CR register */
  {
    statusreg = RCC->CR;
  }
  else          /* The flag to check is in CSR register (tmp == 2) */
  {
    statusreg = RCC->CSR;
  }

  /* Get the flag position */
  tmp = RCC_FLAG & FLAG_MASK;

  if ((statusreg & ((uint32_t)1 << tmp)) != (uint32_t)RESET)
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
