#ifndef _RCC_H
#define _RCC_H

#define RCC_LSE_OFF                      ((uint8_t)0x00)
#define RCC_LSE_ON                       ((uint8_t)0x01)
#define RCC_LSE_Bypass                   ((uint8_t)0x05)

#define RCC_FLAG_HSIRDY                  ((uint8_t)0x21)
#define RCC_FLAG_MSIRDY                  ((uint8_t)0x29)
#define RCC_FLAG_HSERDY                  ((uint8_t)0x31)
#define RCC_FLAG_PLLRDY                  ((uint8_t)0x39)
#define RCC_FLAG_LSERDY                  ((uint8_t)0x49)
#define RCC_FLAG_LSECSS                  ((uint8_t)0x4A)
#define RCC_FLAG_LSIRDY                  ((uint8_t)0x41)
#define RCC_FLAG_OBLRST                  ((uint8_t)0x59)
#define RCC_FLAG_PINRST                  ((uint8_t)0x5A)
#define RCC_FLAG_PORRST                  ((uint8_t)0x5B)
#define RCC_FLAG_SFTRST                  ((uint8_t)0x5C)
#define RCC_FLAG_IWDGRST                 ((uint8_t)0x5D)
#define RCC_FLAG_WWDGRST                 ((uint8_t)0x5E)
#define RCC_FLAG_LPWRRST                 ((uint8_t)0x5F)

#define RCC_RTCCLKSource_LSE             RCC_CSR_RTCSEL_LSE
#define RCC_RTCCLKSource_LSI             RCC_CSR_RTCSEL_LSI
#define RCC_RTCCLKSource_HSE_Div2        RCC_CSR_RTCSEL_HSE
#define RCC_RTCCLKSource_HSE_Div4        ((uint32_t)RCC_CSR_RTCSEL_HSE | RCC_CR_RTCPRE_0)
#define RCC_RTCCLKSource_HSE_Div8        ((uint32_t)RCC_CSR_RTCSEL_HSE | RCC_CR_RTCPRE_1)
#define RCC_RTCCLKSource_HSE_Div16       ((uint32_t)RCC_CSR_RTCSEL_HSE | RCC_CR_RTCPRE)

void RCC_LSEConfig(uint8_t RCC_LSE);

/* Peripheral clocks configuration functions **********************************/
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource);
void RCC_RTCCLKCmd(FunctionalState NewState);
void RCC_RTCResetCmd(FunctionalState NewState);

FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG);

#endif
