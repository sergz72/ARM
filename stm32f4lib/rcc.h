#ifndef _RCC_H
#define _RCC_H

#define RCC_RTCCLKSource_LSE             ((uint32_t)0x00000100)
#define RCC_RTCCLKSource_LSI             ((uint32_t)0x00000200)
#define RCC_RTCCLKSource_HSE_Div2        ((uint32_t)0x00020300)
#define RCC_RTCCLKSource_HSE_Div3        ((uint32_t)0x00030300)
#define RCC_RTCCLKSource_HSE_Div4        ((uint32_t)0x00040300)
#define RCC_RTCCLKSource_HSE_Div5        ((uint32_t)0x00050300)
#define RCC_RTCCLKSource_HSE_Div6        ((uint32_t)0x00060300)
#define RCC_RTCCLKSource_HSE_Div7        ((uint32_t)0x00070300)
#define RCC_RTCCLKSource_HSE_Div8        ((uint32_t)0x00080300)
#define RCC_RTCCLKSource_HSE_Div9        ((uint32_t)0x00090300)
#define RCC_RTCCLKSource_HSE_Div10       ((uint32_t)0x000A0300)
#define RCC_RTCCLKSource_HSE_Div11       ((uint32_t)0x000B0300)
#define RCC_RTCCLKSource_HSE_Div12       ((uint32_t)0x000C0300)
#define RCC_RTCCLKSource_HSE_Div13       ((uint32_t)0x000D0300)
#define RCC_RTCCLKSource_HSE_Div14       ((uint32_t)0x000E0300)
#define RCC_RTCCLKSource_HSE_Div15       ((uint32_t)0x000F0300)
#define RCC_RTCCLKSource_HSE_Div16       ((uint32_t)0x00100300)
#define RCC_RTCCLKSource_HSE_Div17       ((uint32_t)0x00110300)
#define RCC_RTCCLKSource_HSE_Div18       ((uint32_t)0x00120300)
#define RCC_RTCCLKSource_HSE_Div19       ((uint32_t)0x00130300)
#define RCC_RTCCLKSource_HSE_Div20       ((uint32_t)0x00140300)
#define RCC_RTCCLKSource_HSE_Div21       ((uint32_t)0x00150300)
#define RCC_RTCCLKSource_HSE_Div22       ((uint32_t)0x00160300)
#define RCC_RTCCLKSource_HSE_Div23       ((uint32_t)0x00170300)
#define RCC_RTCCLKSource_HSE_Div24       ((uint32_t)0x00180300)
#define RCC_RTCCLKSource_HSE_Div25       ((uint32_t)0x00190300)
#define RCC_RTCCLKSource_HSE_Div26       ((uint32_t)0x001A0300)
#define RCC_RTCCLKSource_HSE_Div27       ((uint32_t)0x001B0300)
#define RCC_RTCCLKSource_HSE_Div28       ((uint32_t)0x001C0300)
#define RCC_RTCCLKSource_HSE_Div29       ((uint32_t)0x001D0300)
#define RCC_RTCCLKSource_HSE_Div30       ((uint32_t)0x001E0300)
#define RCC_RTCCLKSource_HSE_Div31       ((uint32_t)0x001F0300)

#define RCC_PLLSAIDivR_Div2                ((uint32_t)0x00000000)
#define RCC_PLLSAIDivR_Div4                ((uint32_t)0x00010000)
#define RCC_PLLSAIDivR_Div8                ((uint32_t)0x00020000)
#define RCC_PLLSAIDivR_Div16               ((uint32_t)0x00030000)

#define RCC_FLAG_HSIRDY                  ((uint8_t)0x21)
#define RCC_FLAG_HSERDY                  ((uint8_t)0x31)
#define RCC_FLAG_PLLRDY                  ((uint8_t)0x39)
#define RCC_FLAG_PLLI2SRDY               ((uint8_t)0x3B)
#define RCC_FLAG_PLLSAIRDY               ((uint8_t)0x3D)
#define RCC_FLAG_LSERDY                  ((uint8_t)0x41)
#define RCC_FLAG_LSIRDY                  ((uint8_t)0x61)
#define RCC_FLAG_BORRST                  ((uint8_t)0x79)
#define RCC_FLAG_PINRST                  ((uint8_t)0x7A)
#define RCC_FLAG_PORRST                  ((uint8_t)0x7B)
#define RCC_FLAG_SFTRST                  ((uint8_t)0x7C)
#define RCC_FLAG_IWDGRST                 ((uint8_t)0x7D)
#define RCC_FLAG_WWDGRST                 ((uint8_t)0x7E)
#define RCC_FLAG_LPWRRST                 ((uint8_t)0x7F)

#define RCC_AHB1Periph_GPIOA             ((uint32_t)0x00000001)
#define RCC_AHB1Periph_GPIOB             ((uint32_t)0x00000002)
#define RCC_AHB1Periph_GPIOC             ((uint32_t)0x00000004)
#define RCC_AHB1Periph_GPIOD             ((uint32_t)0x00000008)
#define RCC_AHB1Periph_GPIOE             ((uint32_t)0x00000010)
#define RCC_AHB1Periph_GPIOF             ((uint32_t)0x00000020)
#define RCC_AHB1Periph_GPIOG             ((uint32_t)0x00000040)
#define RCC_AHB1Periph_GPIOH             ((uint32_t)0x00000080)
#define RCC_AHB1Periph_GPIOI             ((uint32_t)0x00000100) 
#define RCC_AHB1Periph_GPIOJ             ((uint32_t)0x00000200)
#define RCC_AHB1Periph_GPIOK             ((uint32_t)0x00000400)
#define RCC_AHB1Periph_CRC               ((uint32_t)0x00001000)
#define RCC_AHB1Periph_FLITF             ((uint32_t)0x00008000)
#define RCC_AHB1Periph_SRAM1             ((uint32_t)0x00010000)
#define RCC_AHB1Periph_SRAM2             ((uint32_t)0x00020000)
#define RCC_AHB1Periph_BKPSRAM           ((uint32_t)0x00040000)
#define RCC_AHB1Periph_SRAM3             ((uint32_t)0x00080000)
#define RCC_AHB1Periph_CCMDATARAMEN      ((uint32_t)0x00100000)
#define RCC_AHB1Periph_DMA1              ((uint32_t)0x00200000)
#define RCC_AHB1Periph_DMA2              ((uint32_t)0x00400000)
#define RCC_AHB1Periph_DMA2D             ((uint32_t)0x00800000)
#define RCC_AHB1Periph_ETH_MAC           ((uint32_t)0x02000000)
#define RCC_AHB1Periph_ETH_MAC_Tx        ((uint32_t)0x04000000)
#define RCC_AHB1Periph_ETH_MAC_Rx        ((uint32_t)0x08000000)
#define RCC_AHB1Periph_ETH_MAC_PTP       ((uint32_t)0x10000000)
#define RCC_AHB1Periph_OTG_HS            ((uint32_t)0x20000000)
#define RCC_AHB1Periph_OTG_HS_ULPI       ((uint32_t)0x40000000)
#if defined(STM32F410xx)
#define RCC_AHB1Periph_RNG               ((uint32_t)0x80000000)
#endif /* STM32F410xx */

#define RCC_APB1Periph_TIM2              ((uint32_t)0x00000001)
#define RCC_APB1Periph_TIM3              ((uint32_t)0x00000002)
#define RCC_APB1Periph_TIM4              ((uint32_t)0x00000004)
#define RCC_APB1Periph_TIM5              ((uint32_t)0x00000008)
#define RCC_APB1Periph_TIM6              ((uint32_t)0x00000010)
#define RCC_APB1Periph_TIM7              ((uint32_t)0x00000020)
#define RCC_APB1Periph_TIM12             ((uint32_t)0x00000040)
#define RCC_APB1Periph_TIM13             ((uint32_t)0x00000080)
#define RCC_APB1Periph_TIM14             ((uint32_t)0x00000100)
#if defined(STM32F410xx)
#define RCC_APB1Periph_LPTIM1            ((uint32_t)0x00000200)
#endif /* STM32F410xx */
#define RCC_APB1Periph_WWDG              ((uint32_t)0x00000800)
#define RCC_APB1Periph_SPI2              ((uint32_t)0x00004000)
#define RCC_APB1Periph_SPI3              ((uint32_t)0x00008000)
#if defined(STM32F446xx)
#define RCC_APB1Periph_SPDIFRX           ((uint32_t)0x00010000)
#endif /* STM32F446xx */ 
#define RCC_APB1Periph_USART2            ((uint32_t)0x00020000)
#define RCC_APB1Periph_USART3            ((uint32_t)0x00040000)
#define RCC_APB1Periph_UART4             ((uint32_t)0x00080000)
#define RCC_APB1Periph_UART5             ((uint32_t)0x00100000)
#define RCC_APB1Periph_I2C1              ((uint32_t)0x00200000)
#define RCC_APB1Periph_I2C2              ((uint32_t)0x00400000)
#define RCC_APB1Periph_I2C3              ((uint32_t)0x00800000)
#if defined(STM32F410xx) || defined(STM32F412xG) || defined(STM32F446xx)
#define RCC_APB1Periph_FMPI2C1           ((uint32_t)0x01000000)
#endif /* STM32F410xx || STM32F446xx */ 
#define RCC_APB1Periph_CAN1              ((uint32_t)0x02000000)
#define RCC_APB1Periph_CAN2              ((uint32_t)0x04000000)
#if defined(STM32F446xx)
#define RCC_APB1Periph_CEC               ((uint32_t)0x08000000)
#endif /* STM32F446xx */ 
#define RCC_APB1Periph_PWR               ((uint32_t)0x10000000)
#define RCC_APB1Periph_DAC               ((uint32_t)0x20000000)
#define RCC_APB1Periph_UART7             ((uint32_t)0x40000000)
#define RCC_APB1Periph_UART8             ((uint32_t)0x80000000)

typedef struct
{
  uint32_t SYSCLK_Frequency; /*!<  SYSCLK clock frequency expressed in Hz */
  uint32_t HCLK_Frequency;   /*!<  HCLK clock frequency expressed in Hz   */
  uint32_t PCLK1_Frequency;  /*!<  PCLK1 clock frequency expressed in Hz  */
  uint32_t PCLK2_Frequency;  /*!<  PCLK2 clock frequency expressed in Hz  */
}RCC_ClocksTypeDef;

void RCC_RTCCLKCmd(FunctionalState NewState);
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource);

void        RCC_PLLSAICmd(FunctionalState NewState);
#if defined(STM32F469_479xx)
void        RCC_PLLSAIConfig(uint32_t PLLSAIN, uint32_t PLLSAIP, uint32_t PLLSAIQ, uint32_t PLLSAIR);
#endif /* STM32F469_479xx */
#if defined(STM32F446xx)
void        RCC_PLLSAIConfig(uint32_t PLLSAIM, uint32_t PLLSAIN, uint32_t PLLSAIP, uint32_t PLLSAIQ);
#endif /* STM32F446xx */
#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F429xx) || defined(STM32F401xx) || defined(STM32F411xE)
void        RCC_PLLSAIConfig(uint32_t PLLSAIN, uint32_t PLLSAIQ, uint32_t PLLSAIR);
#endif /* STM32F40_41xxx || STM32F427_437xx || STM32F429_439xx || STM32F401xx || STM32F411xE */

void        RCC_LTDCCLKDivConfig(uint32_t RCC_PLLSAIDivR);
FlagStatus  RCC_GetFlagStatus(uint8_t RCC_FLAG);

void RCC_AHB1PeriphResetCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);

void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks);

#endif
