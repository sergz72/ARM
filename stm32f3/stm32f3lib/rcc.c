#include <board.h>
#include <rcc.h>

/* ------------ RCC registers bit address in the alias region ----------- */
#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)

/* --- CFGR Register ---*/
/* Alias word address of USBPRE bit */
#define CFGR_OFFSET               (RCC_OFFSET + 0x04)
#define USBPRE_BitNumber          0x16
#define CFGR_USBPRE_BB            (PERIPH_BB_BASE + (CFGR_OFFSET * 32) + (USBPRE_BitNumber * 4))

/**
  * @brief  Configures the HRTIM1 clock sources(HRTIM1CLK).
  * @note     The configuration of the HRTIM1 clock source is only possible when the 
  *           SYSCLK = PLL and HCLK and PCLK2 clocks are not divided in respect to SYSCLK
  * @note     If one of the previous conditions is missed, the TIM clock source 
  *           configuration is lost and calling again this function becomes mandatory.  
  * @param  RCC_HRTIMCLK: defines the TIMx clock source.
  *   This parameter can be one of the following values:
  *     @arg RCC_HRTIM1CLK_HCLK: TIMx clock = APB high speed clock (doubled frequency
  *          when prescaled)
  *     @arg RCC_HRTIM1CLK_PLLCLK: TIMx clock = PLL output (running up to 144 MHz)
  *          (x can be 1 or 8).
  * @retval None
  */
#ifdef RCC_CFGR3_HRTIM1SW
void RCC_HRTIM1CLKConfig(uint32_t RCC_HRTIMCLK)
{ 
  /* Clear HRTIMSW bit */
  RCC->CFGR3 &= ~RCC_CFGR3_HRTIM1SW;

  /* Set HRTIMSW bits according to RCC_HRTIMCLK value */
  RCC->CFGR3 |= RCC_HRTIMCLK;
}
#endif

/**
  * @brief  Configures the ADC clock (ADCCLK).
  * @param  RCC_PLLCLK: defines the ADC clock divider. This clock is derived from 
  *         the PLL Clock.
  *   This parameter can be one of the following values:
  *     @arg RCC_ADC12PLLCLK_OFF: ADC12 clock disabled
  *     @arg RCC_ADC12PLLCLK_Div1: ADC12 clock = PLLCLK/1
  *     @arg RCC_ADC12PLLCLK_Div2: ADC12 clock = PLLCLK/2
  *     @arg RCC_ADC12PLLCLK_Div4: ADC12 clock = PLLCLK/4
  *     @arg RCC_ADC12PLLCLK_Div6: ADC12 clock = PLLCLK/6
  *     @arg RCC_ADC12PLLCLK_Div8: ADC12 clock = PLLCLK/8
  *     @arg RCC_ADC12PLLCLK_Div10: ADC12 clock = PLLCLK/10
  *     @arg RCC_ADC12PLLCLK_Div12: ADC12 clock = PLLCLK/12
  *     @arg RCC_ADC12PLLCLK_Div16: ADC12 clock = PLLCLK/16
  *     @arg RCC_ADC12PLLCLK_Div32: ADC12 clock = PLLCLK/32
  *     @arg RCC_ADC12PLLCLK_Div64: ADC12 clock = PLLCLK/64
  *     @arg RCC_ADC12PLLCLK_Div128: ADC12 clock = PLLCLK/128
  *     @arg RCC_ADC12PLLCLK_Div256: ADC12 clock = PLLCLK/256
  *     @arg RCC_ADC34PLLCLK_OFF: ADC34 clock disabled
  *     @arg RCC_ADC34PLLCLK_Div1: ADC34 clock = PLLCLK/1
  *     @arg RCC_ADC34PLLCLK_Div2: ADC34 clock = PLLCLK/2
  *     @arg RCC_ADC34PLLCLK_Div4: ADC34 clock = PLLCLK/4
  *     @arg RCC_ADC34PLLCLK_Div6: ADC34 clock = PLLCLK/6
  *     @arg RCC_ADC34PLLCLK_Div8: ADC34 clock = PLLCLK/8
  *     @arg RCC_ADC34PLLCLK_Div10: ADC34 clock = PLLCLK/10
  *     @arg RCC_ADC34PLLCLK_Div12: ADC34 clock = PLLCLK/12
  *     @arg RCC_ADC34PLLCLK_Div16: ADC34 clock = PLLCLK/16
  *     @arg RCC_ADC34PLLCLK_Div32: ADC34 clock = PLLCLK/32
  *     @arg RCC_ADC34PLLCLK_Div64: ADC34 clock = PLLCLK/64       
  *     @arg RCC_ADC34PLLCLK_Div128: ADC34 clock = PLLCLK/128                                  
  *     @arg RCC_ADC34PLLCLK_Div256: ADC34 clock = PLLCLK/256
  * @retval None
  */
void RCC_ADCCLKConfig(uint32_t RCC_PLLCLK)
{
  uint32_t tmp = 0;
  
  tmp = (RCC_PLLCLK >> 28);
  
  /* Clears ADCPRE34 bits */
  if (tmp != 0)
  {
#ifdef RCC_CFGR2_ADCPRE34
    RCC->CFGR2 &= ~RCC_CFGR2_ADCPRE34;
#endif
  }
   /* Clears ADCPRE12 bits */
  else
  {
    RCC->CFGR2 &= ~RCC_CFGR2_ADCPRE12;
  }
  /* Set ADCPRE bits according to RCC_PLLCLK value */
  RCC->CFGR2 |= RCC_PLLCLK;
}

/**
  * @brief  Configures the USB clock (USBCLK).
  * @param  RCC_USBCLKSource: specifies the USB clock source. This clock is 
  *   derived from the PLL output.
  *   This parameter can be one of the following values:
  *     @arg RCC_USBCLKSource_PLLCLK_1Div5: PLL clock divided by 1,5 selected as USB 
  *                                     clock source
  *     @arg RCC_USBCLKSource_PLLCLK_Div1: PLL clock selected as USB clock source
  * @retval None
  */
void RCC_USBCLKConfig(uint32_t RCC_USBCLKSource)
{
  *(__IO uint32_t *) CFGR_USBPRE_BB = RCC_USBCLKSource;
}
