#ifndef _DBGMCU_H
#define _DBGMCU_H

#define DBGMCU_SLEEP                 ((uint32_t)0x00000001)
#define DBGMCU_STOP                  ((uint32_t)0x00000002)
#define DBGMCU_STANDBY               ((uint32_t)0x00000004)
#define IS_DBGMCU_PERIPH(PERIPH) ((((PERIPH) & 0xFFFFFFF8) == 0x00) && ((PERIPH) != 0x00))

#define DBGMCU_TIM2_STOP             ((uint32_t)0x00000001)
#define DBGMCU_TIM3_STOP             ((uint32_t)0x00000002)
#define DBGMCU_TIM4_STOP             ((uint32_t)0x00000004)
#define DBGMCU_TIM5_STOP             ((uint32_t)0x00000008)
#define DBGMCU_TIM6_STOP             ((uint32_t)0x00000010)
#define DBGMCU_TIM7_STOP             ((uint32_t)0x00000020)
#define DBGMCU_RTC_STOP              ((uint32_t)0x00000400)
#define DBGMCU_WWDG_STOP             ((uint32_t)0x00000800)
#define DBGMCU_IWDG_STOP             ((uint32_t)0x00001000)
#define DBGMCU_I2C1_SMBUS_TIMEOUT    ((uint32_t)0x00200000)
#define DBGMCU_I2C2_SMBUS_TIMEOUT    ((uint32_t)0x00400000)
#define IS_DBGMCU_APB1PERIPH(PERIPH) ((((PERIPH) & 0xFF9FE3C0) == 0x00) && ((PERIPH) != 0x00))

#define DBGMCU_TIM9_STOP             ((uint32_t)0x00000004)
#define DBGMCU_TIM10_STOP            ((uint32_t)0x00000008)
#define DBGMCU_TIM11_STOP            ((uint32_t)0x00000010)
#define IS_DBGMCU_APB2PERIPH(PERIPH) ((((PERIPH) & 0xFFFFFFE3) == 0x00) && ((PERIPH) != 0x00))

uint32_t DBGMCU_GetREVID(void);
uint32_t DBGMCU_GetDEVID(void);
void DBGMCU_Config(uint32_t DBGMCU_Periph, FunctionalState NewState);
void DBGMCU_APB1PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState);
void DBGMCU_APB2PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState);

#endif /* _DBGMCU_H */
