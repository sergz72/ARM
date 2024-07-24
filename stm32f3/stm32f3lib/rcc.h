#ifndef _RCC_H
#define _RCC_H

/** @defgroup RCC_HRTIM_clock_source 
  * @{
  */

#define RCC_HRTIM1CLK_HCLK                  ((uint32_t)0x00000000)
#define RCC_HRTIM1CLK_PLLCLK                RCC_CFGR3_HRTIM1SW

/* ADC1 & ADC2 */
#define RCC_ADC12PLLCLK_OFF                    ((uint32_t)0x00000000)
#define RCC_ADC12PLLCLK_Div1                   ((uint32_t)0x00000100)
#define RCC_ADC12PLLCLK_Div2                   ((uint32_t)0x00000110)
#define RCC_ADC12PLLCLK_Div4                   ((uint32_t)0x00000120)
#define RCC_ADC12PLLCLK_Div6                   ((uint32_t)0x00000130)
#define RCC_ADC12PLLCLK_Div8                   ((uint32_t)0x00000140)
#define RCC_ADC12PLLCLK_Div10                  ((uint32_t)0x00000150)
#define RCC_ADC12PLLCLK_Div12                  ((uint32_t)0x00000160)
#define RCC_ADC12PLLCLK_Div16                  ((uint32_t)0x00000170)
#define RCC_ADC12PLLCLK_Div32                  ((uint32_t)0x00000180)
#define RCC_ADC12PLLCLK_Div64                  ((uint32_t)0x00000190)
#define RCC_ADC12PLLCLK_Div128                 ((uint32_t)0x000001A0)
#define RCC_ADC12PLLCLK_Div256                 ((uint32_t)0x000001B0)

#define RCC_USBCLKSource_PLLCLK_1Div5   ((uint8_t)0x00)
#define RCC_USBCLKSource_PLLCLK_Div1    ((uint8_t)0x01)

void RCC_HRTIM1CLKConfig(uint32_t RCC_HRTIMCLK);
void RCC_ADCCLKConfig(uint32_t RCC_PLLCLK);
void RCC_USBCLKConfig(uint32_t RCC_USBCLKSource);

#endif
