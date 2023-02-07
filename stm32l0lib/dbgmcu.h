#ifndef _DBGMCU_H
#define _DBGMCU_H

#define DBGMCU_SLEEP                 ((unsigned int)0x00000001)
#define DBGMCU_STOP                  ((unsigned int)0x00000002)
#define DBGMCU_STANDBY               ((unsigned int)0x00000004)

#define DBGMCU_APB1_TIM2_STOP             ((unsigned int)0x00000001)
#define DBGMCU_APB1_TIM3_STOP             ((unsigned int)0x00000002)
#define DBGMCU_APB1_TIM4_STOP             ((unsigned int)0x00000004)
#define DBGMCU_APB1_TIM5_STOP             ((unsigned int)0x00000008)
#define DBGMCU_APB1_TIM6_STOP             ((unsigned int)0x00000010)
#define DBGMCU_APB1_TIM7_STOP             ((unsigned int)0x00000020)
#define DBGMCU_APB1_RTC_STOP              ((unsigned int)0x00000400)
#define DBGMCU_APB1_WWDG_STOP             ((unsigned int)0x00000800)
#define DBGMCU_APB1_IWDG_STOP             ((unsigned int)0x00001000)
#define DBGMCU_APB1_I2C1_SMBUS_TIMEOUT    ((unsigned int)0x00200000)
#define DBGMCU_APB1_I2C2_SMBUS_TIMEOUT    ((unsigned int)0x00400000)

#define DBGMCU_APB2_TIM9_STOP             ((unsigned int)0x00000004)
#define DBGMCU_APB2_TIM10_STOP            ((unsigned int)0x00000008)
#define DBGMCU_APB2_TIM11_STOP            ((unsigned int)0x00000010)

unsigned int DBGMCU_GetREVID(void);
unsigned int DBGMCU_GetDEVID(void);
void DBGMCU_Config(unsigned int DBGMCU_Periph, FunctionalState NewState);
void DBGMCU_APB1PeriphConfig(unsigned int DBGMCU_Periph, FunctionalState NewState);
void DBGMCU_APB2PeriphConfig(unsigned int DBGMCU_Periph, FunctionalState NewState);

#endif /* _DBGMCU_H */
