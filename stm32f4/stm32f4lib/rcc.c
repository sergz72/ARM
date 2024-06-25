#include "board.h"
#include "rcc.h"

static const uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

/* Private define ------------------------------------------------------------*/
/* ------------ RCC registers bit address in the alias region ----------- */
#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)
/* --- CR Register ---*/
/* Alias word address of HSION bit */
#define CR_OFFSET                 (RCC_OFFSET + 0x00)
#define HSION_BitNumber           0x00
#define CR_HSION_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (HSION_BitNumber * 4))
/* Alias word address of CSSON bit */
#define CSSON_BitNumber           0x13
#define CR_CSSON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (CSSON_BitNumber * 4))
/* Alias word address of PLLON bit */
#define PLLON_BitNumber           0x18
#define CR_PLLON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLON_BitNumber * 4))
/* Alias word address of PLLI2SON bit */
#define PLLI2SON_BitNumber        0x1A
#define CR_PLLI2SON_BB            (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLI2SON_BitNumber * 4))

/* Alias word address of PLLSAION bit */
#define PLLSAION_BitNumber        0x1C
#define CR_PLLSAION_BB            (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLSAION_BitNumber * 4))

/* --- CFGR Register ---*/
/* Alias word address of I2SSRC bit */
#define CFGR_OFFSET               (RCC_OFFSET + 0x08)
#define I2SSRC_BitNumber          0x17
#define CFGR_I2SSRC_BB            (PERIPH_BB_BASE + (CFGR_OFFSET * 32) + (I2SSRC_BitNumber * 4))

/* --- BDCR Register ---*/
/* Alias word address of RTCEN bit */
#define BDCR_OFFSET               (RCC_OFFSET + 0x70)
#define RTCEN_BitNumber           0x0F
#define BDCR_RTCEN_BB             (PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (RTCEN_BitNumber * 4))
/* Alias word address of BDRST bit */
#define BDRST_BitNumber           0x10
#define BDCR_BDRST_BB             (PERIPH_BB_BASE + (BDCR_OFFSET * 32) + (BDRST_BitNumber * 4))

/* --- CSR Register ---*/
/* Alias word address of LSION bit */
#define CSR_OFFSET                (RCC_OFFSET + 0x74)
#define LSION_BitNumber           0x00
#define CSR_LSION_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (LSION_BitNumber * 4))

/* --- DCKCFGR Register ---*/
/* Alias word address of TIMPRE bit */
#define DCKCFGR_OFFSET            (RCC_OFFSET + 0x8C)
#define TIMPRE_BitNumber          0x18
#define DCKCFGR_TIMPRE_BB         (PERIPH_BB_BASE + (DCKCFGR_OFFSET * 32) + (TIMPRE_BitNumber * 4))
    
/* --- CFGR Register ---*/
#define RCC_CFGR_OFFSET            (RCC_OFFSET + 0x08)
 #if defined(STM32F410xx)
/* Alias word address of MCO1EN bit */
#define RCC_MCO1EN_BIT_NUMBER      0x8
#define RCC_CFGR_MCO1EN_BB         (PERIPH_BB_BASE + (RCC_CFGR_OFFSET * 32) + (RCC_MCO1EN_BIT_NUMBER * 4))

/* Alias word address of MCO2EN bit */
#define RCC_MCO2EN_BIT_NUMBER      0x9
#define RCC_CFGR_MCO2EN_BB         (PERIPH_BB_BASE + (RCC_CFGR_OFFSET * 32) + (RCC_MCO2EN_BIT_NUMBER * 4))
#endif /* STM32F410xx */
/* ---------------------- RCC registers bit mask ------------------------ */
/* CFGR register bit mask */
#define CFGR_MCO2_RESET_MASK      ((uint32_t)0x07FFFFFF)
#define CFGR_MCO1_RESET_MASK      ((uint32_t)0xF89FFFFF)

/* RCC Flag Mask */
#define FLAG_MASK                 ((uint8_t)0x1F)

/* CR register byte 3 (Bits[23:16]) base address */
#define CR_BYTE3_ADDRESS          ((uint32_t)0x40023802)

/* CIR register byte 2 (Bits[15:8]) base address */
#define CIR_BYTE2_ADDRESS         ((uint32_t)(RCC_BASE + 0x0C + 0x01))

/* CIR register byte 3 (Bits[23:16]) base address */
#define CIR_BYTE3_ADDRESS         ((uint32_t)(RCC_BASE + 0x0C + 0x02))

/* BDCR register base address */
#define BDCR_ADDRESS              (PERIPH_BASE + BDCR_OFFSET)

/**
  * @brief  Enables or disables the RTC clock.
  * @note   This function must be used only after the RTC clock source was selected
  *         using the RCC_RTCCLKConfig function.
  * @param  NewState: new state of the RTC clock. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_RTCCLKCmd(FunctionalState NewState)
{
  *(__IO uint32_t *) BDCR_RTCEN_BB = (uint32_t)NewState;
}

/**
  * @brief  Configures the RTC clock (RTCCLK).
  * @note   As the RTC clock configuration bits are in the Backup domain and write
  *         access is denied to this domain after reset, you have to enable write
  *         access using PWR_BackupAccessCmd(ENABLE) function before to configure
  *         the RTC clock source (to be done once after reset).    
  * @note   Once the RTC clock is configured it can't be changed unless the  
  *         Backup domain is reset using RCC_BackupResetCmd() function, or by
  *         a Power On Reset (POR).
  *    
  * @param  RCC_RTCCLKSource: specifies the RTC clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_RTCCLKSource_LSE: LSE selected as RTC clock
  *            @arg RCC_RTCCLKSource_LSI: LSI selected as RTC clock
  *            @arg RCC_RTCCLKSource_HSE_Divx: HSE clock divided by x selected
  *                                            as RTC clock, where x:[2,31]
  *  
  * @note   If the LSE or LSI is used as RTC clock source, the RTC continues to
  *         work in STOP and STANDBY modes, and can be used as wakeup source.
  *         However, when the HSE clock is used as RTC clock source, the RTC
  *         cannot be used in STOP and STANDBY modes.    
  * @note   The maximum input clock frequency for RTC is 1MHz (when using HSE as
  *         RTC clock source).
  *  
  * @retval None
  */
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource)
{
  uint32_t tmpreg = 0;

  if ((RCC_RTCCLKSource & 0x00000300) == 0x00000300)
  { /* If HSE is selected as RTC clock source, configure HSE division factor for RTC clock */
    tmpreg = RCC->CFGR;

    /* Clear RTCPRE[4:0] bits */
    tmpreg &= ~RCC_CFGR_RTCPRE;

    /* Configure HSE division factor for RTC clock */
    tmpreg |= (RCC_RTCCLKSource & 0xFFFFCFF);

    /* Store the new value */
    RCC->CFGR = tmpreg;
  }
    
  /* Select the RTC clock source */
  RCC->BDCR |= (RCC_RTCCLKSource & 0x00000FFF);
}

#if defined(STM32F469_479xx)
/**
  * @brief  Configures the PLLSAI clock multiplication and division factors.
  *
  * @note   This function can be used only for STM32F469_479xx devices 
  *        
  * @note   This function must be used only when the PLLSAI is disabled.
  * @note   PLLSAI clock source is common with the main PLL (configured in 
  *         RCC_PLLConfig function )  
  *
  * @param  PLLSAIN: specifies the multiplication factor for PLLSAI VCO output clock
  *         This parameter must be a number between 50 and 432.
  * @note   You have to set the PLLSAIN parameter correctly to ensure that the VCO 
  *         output frequency is between 100 and 432 MHz.
  *
  * @param  PLLSAIP: specifies the division factor for PLL 48Mhz clock output
  *         This parameter must be a number in the range {2, 4, 6, or 8}..
  *           
  * @param  PLLSAIQ: specifies the division factor for SAI1 clock
  *         This parameter must be a number between 2 and 15.
  *            
  * @param  PLLSAIR: specifies the division factor for LTDC clock
  *          This parameter must be a number between 2 and 7.
  *   
  * @retval None
  */
void RCC_PLLSAIConfig(uint32_t PLLSAIN, uint32_t PLLSAIP, uint32_t PLLSAIQ, uint32_t PLLSAIR)
{
  RCC->PLLSAICFGR = (PLLSAIN << 6) | (((PLLSAIP >> 1) -1) << 16) | (PLLSAIQ << 24) | (PLLSAIR << 28);
}
#endif /* STM32F469_479xx */

#if defined(STM32F446xx)
/**
  * @brief  Configures the PLLSAI clock multiplication and division factors.
  *
  * @note   This function can be used only for STM32F446xx devices 
  *        
  * @note   This function must be used only when the PLLSAI is disabled.
  * @note   PLLSAI clock source is common with the main PLL (configured in 
  *         RCC_PLLConfig function )  
  * 
  * @param  PLLSAIM: specifies the division factor for PLLSAI VCO input clock
  *         This parameter must be a number between Min_Data = 2 and Max_Data = 63.
  * @note   You have to set the PLLSAIM parameter correctly to ensure that the VCO input
  *         frequency ranges from 1 to 2 MHz. It is recommended to select a frequency
  *         of 2 MHz to limit PLLSAI jitter.
  *
  * @param  PLLSAIN: specifies the multiplication factor for PLLSAI VCO output clock
  *         This parameter must be a number between 50 and 432.
  * @note   You have to set the PLLSAIN parameter correctly to ensure that the VCO 
  *         output frequency is between 100 and 432 MHz.
  * 
  * @param  PLLSAIP: specifies the division factor for PLL 48Mhz clock output
  *         This parameter must be a number in the range {2, 4, 6, or 8}.
  *
  * @param  PLLSAIQ: specifies the division factor for SAI1 clock
  *         This parameter must be a number between 2 and 15.
  *   
  * @retval None
  */
void RCC_PLLSAIConfig(uint32_t PLLSAIM, uint32_t PLLSAIN, uint32_t PLLSAIP, uint32_t PLLSAIQ)
{
  RCC->PLLSAICFGR = PLLSAIM | (PLLSAIN << 6) | (((PLLSAIP >> 1) -1) << 16)  | (PLLSAIQ << 24);
}
#endif /* STM32F446xx */

#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F429xx) || defined(STM32F401xx) || defined(STM32F411xE)
/**
  * @brief  Configures the PLLSAI clock multiplication and division factors.
  *
  * @note   This function can be used only for STM32F42xxx/43xxx devices 
  *        
  * @note   This function must be used only when the PLLSAI is disabled.
  * @note   PLLSAI clock source is common with the main PLL (configured in 
  *         RCC_PLLConfig function )  
  *             
  * @param  PLLSAIN: specifies the multiplication factor for PLLSAI VCO output clock
  *          This parameter must be a number between 50 and 432.
  * @note   You have to set the PLLSAIN parameter correctly to ensure that the VCO 
  *         output frequency is between 100 and 432 MHz.
  *           
  * @param  PLLSAIQ: specifies the division factor for SAI1 clock
  *          This parameter must be a number between 2 and 15.
  *            
  * @param  PLLSAIR: specifies the division factor for LTDC clock
  *          This parameter must be a number between 2 and 7.
  *   
  * @retval None
  */
void RCC_PLLSAIConfig(uint32_t PLLSAIN, uint32_t PLLSAIQ, uint32_t PLLSAIR)
{
  RCC->PLLSAICFGR = (PLLSAIN << 6) | (PLLSAIQ << 24) | (PLLSAIR << 28);
}
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429_439xx || STM32F401xx || STM32F411xE */

/**
  * @brief  Configures the LTDC clock Divider coming from PLLSAI.
  * 
  * @note   The LTDC peripheral is only available with STM32F42xxx/43xxx/446xx/469xx/479xx Devices.
  *      
  * @note   This function must be called before enabling the PLLSAI.
  *   
  * @param  RCC_PLLSAIDivR: specifies the PLLSAI division factor for LTDC clock .
  *          LTDC clock frequency = f(PLLSAI_R) / RCC_PLLSAIDivR  
  *          This parameter can be one of the following values:
  *            @arg RCC_PLLSAIDivR_Div2: LTDC clock = f(PLLSAI_R)/2
  *            @arg RCC_PLLSAIDivR_Div4: LTDC clock = f(PLLSAI_R)/4
  *            @arg RCC_PLLSAIDivR_Div8: LTDC clock = f(PLLSAI_R)/8
  *            @arg RCC_PLLSAIDivR_Div16: LTDC clock = f(PLLSAI_R)/16
  *            
  * @retval None
  */
#ifdef RCC_DCKCFGR_PLLSAIDIVR
void RCC_LTDCCLKDivConfig(uint32_t RCC_PLLSAIDivR)
{
  uint32_t tmpreg = 0;
  
  tmpreg = RCC->DCKCFGR;
  /* Clear PLLSAIDIVR[2:0] bits */
  tmpreg &= ~RCC_DCKCFGR_PLLSAIDIVR;

  /* Set PLLSAIDIVR values */
  tmpreg |= RCC_PLLSAIDivR;

  /* Store the new value */
  RCC->DCKCFGR = tmpreg;
}
#endif

/**
  * @brief  Enables or disables the PLLSAI. 
  * 
  * @note   This function can be used only for STM32F42xxx/43xxx/446xx/469xx/479xx devices 
  *       
  * @note   The PLLSAI is disabled by hardware when entering STOP and STANDBY modes.  
  * @param  NewState: new state of the PLLSAI. This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_PLLSAICmd(FunctionalState NewState)
{
  *(__IO uint32_t *) CR_PLLSAION_BB = (uint32_t)NewState;
}

/**
  * @brief  Checks whether the specified RCC flag is set or not.
  * @param  RCC_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready
  *            @arg RCC_FLAG_HSERDY: HSE oscillator clock ready
  *            @arg RCC_FLAG_PLLRDY: main PLL clock ready
  *            @arg RCC_FLAG_PLLI2SRDY: PLLI2S clock ready
  *            @arg RCC_FLAG_PLLSAIRDY: PLLSAI clock ready (only for STM32F42xxx/43xxx/446xx/469xx/479xx devices)
  *            @arg RCC_FLAG_LSERDY: LSE oscillator clock ready
  *            @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready
  *            @arg RCC_FLAG_BORRST: POR/PDR or BOR reset
  *            @arg RCC_FLAG_PINRST: Pin reset
  *            @arg RCC_FLAG_PORRST: POR/PDR reset
  *            @arg RCC_FLAG_SFTRST: Software reset
  *            @arg RCC_FLAG_IWDGRST: Independent Watchdog reset
  *            @arg RCC_FLAG_WWDGRST: Window Watchdog reset
  *            @arg RCC_FLAG_LPWRRST: Low Power reset
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
  else if (tmp == 2)          /* The flag to check is in BDCR register */
  {
    statusreg = RCC->BDCR;
  }
  else                       /* The flag to check is in CSR register */
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

/**
  * @brief  Forces or releases AHB1 peripheral reset.
  * @param  RCC_AHB1Periph: specifies the AHB1 peripheral to reset.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_AHB1Periph_GPIOA:   GPIOA clock
  *            @arg RCC_AHB1Periph_GPIOB:   GPIOB clock 
  *            @arg RCC_AHB1Periph_GPIOC:   GPIOC clock
  *            @arg RCC_AHB1Periph_GPIOD:   GPIOD clock
  *            @arg RCC_AHB1Periph_GPIOE:   GPIOE clock
  *            @arg RCC_AHB1Periph_GPIOF:   GPIOF clock
  *            @arg RCC_AHB1Periph_GPIOG:   GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOG:   GPIOG clock
  *            @arg RCC_AHB1Periph_GPIOI:   GPIOI clock
  *            @arg RCC_AHB1Periph_GPIOJ:   GPIOJ clock (STM32F42xxx/43xxx devices) 
  *            @arg RCC_AHB1Periph_GPIOK:   GPIOK clock (STM32F42xxx/43xxxdevices)   
  *            @arg RCC_AHB1Periph_CRC:     CRC clock
  *            @arg RCC_AHB1Periph_DMA1:    DMA1 clock
  *            @arg RCC_AHB1Periph_DMA2:    DMA2 clock
  *            @arg RCC_AHB1Periph_DMA2D:   DMA2D clock (STM32F429xx/439xx devices)   
  *            @arg RCC_AHB1Periph_ETH_MAC: Ethernet MAC clock
  *            @arg RCC_AHB1Periph_OTG_HS:  USB OTG HS clock
  *            @arg RCC_AHB1Periph_RNG:    RNG clock for STM32F410xx devices
  *                  
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHB1PeriphResetCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    RCC->AHB1RSTR |= RCC_AHB1Periph;
  }
  else
  {
    RCC->AHB1RSTR &= ~RCC_AHB1Periph;
  }
}

/**
  * @brief  Returns the frequencies of different on chip clocks; SYSCLK, HCLK, 
  *         PCLK1 and PCLK2.
  * 
  * @note   The system frequency computed by this function is not the real 
  *         frequency in the chip. It is calculated based on the predefined 
  *         constant and the selected clock source:
  * @note     If SYSCLK source is HSI, function returns values based on HSI_VALUE(*)
  * @note     If SYSCLK source is HSE, function returns values based on HSE_VALUE(**)
  * @note     If SYSCLK source is PLL, function returns values based on HSE_VALUE(**) 
  *           or HSI_VALUE(*) multiplied/divided by the PLL factors.         
  * @note     (*) HSI_VALUE is a constant defined in stm32f4xx.h file (default value
  *               16 MHz) but the real value may vary depending on the variations
  *               in voltage and temperature.
  * @note     (**) HSE_VALUE is a constant defined in stm32f4xx.h file (default value
  *                25 MHz), user has to ensure that HSE_VALUE is same as the real
  *                frequency of the crystal used. Otherwise, this function may
  *                have wrong result.
  *                
  * @note   The result of this function could be not correct when using fractional
  *         value for HSE crystal.
  *   
  * @param  RCC_Clocks: pointer to a RCC_ClocksTypeDef structure which will hold
  *          the clocks frequencies.
  *     
  * @note   This function can be used by the user application to compute the 
  *         baudrate for the communication peripherals or configure other parameters.
  * @note   Each time SYSCLK, HCLK, PCLK1 and/or PCLK2 clock changes, this function
  *         must be called to update the structure's field. Otherwise, any
  *         configuration based on this function will be incorrect.
  *    
  * @retval None
  */
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks)
{
  uint32_t tmp = 0, presc = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;
#if defined(STM32F412xG) || defined(STM32F446xx)  
  uint32_t pllr = 2;
#endif /* STM32F412xG || STM32F446xx */
  
  /* Get SYSCLK source -------------------------------------------------------*/
  tmp = RCC->CFGR & RCC_CFGR_SWS;
  
  switch (tmp)
  {
  case 0x00:  /* HSI used as system clock source */
    RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
    break;
  case 0x04:  /* HSE used as system clock  source */
    RCC_Clocks->SYSCLK_Frequency = HSE_VALUE;
    break;
  case 0x08:  /* PLL P used as system clock  source */
    
    /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLLM) * PLLN
    SYSCLK = PLL_VCO / PLLP
    */    
    pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
    pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
    
    if (pllsource != 0)
    {
      /* HSE used as PLL clock source */
      pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
    }
    else
    {
      /* HSI used as PLL clock source */
      pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);      
    }
    
    pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
    RCC_Clocks->SYSCLK_Frequency = pllvco/pllp;
    break;

#if defined(STM32F412xG) || defined(STM32F446xx)
  case 0x0C:  /* PLL R used as system clock  source */
    /* PLL_VCO = (HSE_VALUE or HSI_VALUE / PLLM) * PLLN
    SYSCLK = PLL_VCO / PLLR
    */    
    pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
    pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
    
    if (pllsource != 0)
    {
      /* HSE used as PLL clock source */
      pllvco = (HSE_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
    }
    else
    {
      /* HSI used as PLL clock source */
      pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);      
    }
    
    pllr = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLR) >>28) + 1 ) *2;
    RCC_Clocks->SYSCLK_Frequency = pllvco/pllr;    
    break;
#endif /* STM32F412xG || STM32F446xx */
    
  default:
    RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
    break;
  }
  /* Compute HCLK, PCLK1 and PCLK2 clocks frequencies ------------------------*/
  
  /* Get HCLK prescaler */
  tmp = RCC->CFGR & RCC_CFGR_HPRE;
  tmp = tmp >> 4;
  presc = APBAHBPrescTable[tmp];
  /* HCLK clock frequency */
  RCC_Clocks->HCLK_Frequency = RCC_Clocks->SYSCLK_Frequency >> presc;

  /* Get PCLK1 prescaler */
  tmp = RCC->CFGR & RCC_CFGR_PPRE1;
  tmp = tmp >> 10;
  presc = APBAHBPrescTable[tmp];
  /* PCLK1 clock frequency */
  RCC_Clocks->PCLK1_Frequency = RCC_Clocks->HCLK_Frequency >> presc;

  /* Get PCLK2 prescaler */
  tmp = RCC->CFGR & RCC_CFGR_PPRE2;
  tmp = tmp >> 13;
  presc = APBAHBPrescTable[tmp];
  /* PCLK2 clock frequency */
  RCC_Clocks->PCLK2_Frequency = RCC_Clocks->HCLK_Frequency >> presc;
}

/**
  * @brief  Forces or releases Low Speed APB (APB1) peripheral reset.
  * @param  RCC_APB1Periph: specifies the APB1 peripheral to reset.
  *          This parameter can be any combination of the following values:
  *            @arg RCC_APB1Periph_TIM2:   TIM2 clock
  *            @arg RCC_APB1Periph_TIM3:   TIM3 clock
  *            @arg RCC_APB1Periph_TIM4:   TIM4 clock
  *            @arg RCC_APB1Periph_TIM5:   TIM5 clock
  *            @arg RCC_APB1Periph_TIM6:   TIM6 clock
  *            @arg RCC_APB1Periph_TIM7:   TIM7 clock
  *            @arg RCC_APB1Periph_TIM12:  TIM12 clock
  *            @arg RCC_APB1Periph_TIM13:  TIM13 clock
  *            @arg RCC_APB1Periph_TIM14:  TIM14 clock
  *            @arg RCC_APB1Periph_LPTIM1: LPTIM1 clock (STM32F410xx devices) 
  *            @arg RCC_APB1Periph_WWDG:   WWDG clock
  *            @arg RCC_APB1Periph_SPI2:   SPI2 clock
  *            @arg RCC_APB1Periph_SPI3:   SPI3 clock
  *            @arg RCC_APB1Periph_SPDIF:   SPDIF RX clock (STM32F446xx devices) 
  *            @arg RCC_APB1Periph_USART2: USART2 clock
  *            @arg RCC_APB1Periph_USART3: USART3 clock
  *            @arg RCC_APB1Periph_UART4:  UART4 clock
  *            @arg RCC_APB1Periph_UART5:  UART5 clock
  *            @arg RCC_APB1Periph_I2C1:   I2C1 clock
  *            @arg RCC_APB1Periph_I2C2:   I2C2 clock
  *            @arg RCC_APB1Periph_I2C3:   I2C3 clock
  *            @arg RCC_APB1Periph_FMPI2C1:   FMPI2C1 clock
  *            @arg RCC_APB1Periph_CAN1:   CAN1 clock
  *            @arg RCC_APB1Periph_CAN2:   CAN2 clock
  *            @arg RCC_APB1Periph_CEC:    CEC clock(STM32F446xx devices)
  *            @arg RCC_APB1Periph_PWR:    PWR clock
  *            @arg RCC_APB1Periph_DAC:    DAC clock
  *            @arg RCC_APB1Periph_UART7:  UART7 clock
  *            @arg RCC_APB1Periph_UART8:  UART8 clock  
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    RCC->APB1RSTR |= RCC_APB1Periph;
  }
  else
  {
    RCC->APB1RSTR &= ~RCC_APB1Periph;
  }
}
