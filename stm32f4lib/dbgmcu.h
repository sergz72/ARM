#ifndef _DBGMCU_H
#define _DBGMCU_H

#define DBGMCU_SLEEP                 0x00000001U
#define DBGMCU_STOP                  0x00000002U
#define DBGMCU_STANDBY               0x00000004U

#define DBGMCU_APB11_TIM2_STOP             0x00000001U
#define DBGMCU_APB11_TIM3_STOP             0x00000002U
#define DBGMCU_APB11_TIM4_STOP             0x00000004U
#define DBGMCU_APB11_TIM5_STOP             0x00000008U
#define DBGMCU_APB11_TIM6_STOP             0x00000010U
#define DBGMCU_APB11_TIM7_STOP             0x00000020U
#define DBGMCU_APB11_RTC_STOP              0x00000400U
#define DBGMCU_APB11_WWDG_STOP             0x00000800U
#define DBGMCU_APB11_IWDG_STOP             0x00001000U
#define DBGMCU_APB11_I2C1_STOP             0x00200000U
#define DBGMCU_APB11_I2C2_STOP             0x00400000U
#define DBGMCU_APB11_I2C3_STOP             0x00800000U
#define DBGMCU_APB11_CAN1_STOP             0x02000000U
#define DBGMCU_APB11_CAN2_STOP             0x04000000U
#define DBGMCU_APB11_LPTIM1_STOP           0x80000000U

#define DBGMCU_APB12_I2C4_STOP           2U
#define DBGMCU_APB12_LPTIM2_STOP         0x20U

#define DBGMCU_APB2_TIM1_STOP  0x800U
#define DBGMCU_APB2_TIM8_STOP  0x2000U
#define DBGMCU_APB2_TIM15_STOP 0x10000U
#define DBGMCU_APB2_TIM16_STOP 0x20000U
#define DBGMCU_APB2_TIM17_STOP 0x40000U

void DBGMCU_Config(unsigned int DBGMCU_Periph, FunctionalState NewState);
void DBGMCU_APB11PeriphConfig(unsigned int DBGMCU_Periph, FunctionalState NewState);
void DBGMCU_APB12PeriphConfig(unsigned int DBGMCU_Periph, FunctionalState NewState);
void DBGMCU_APB2PeriphConfig(unsigned int DBGMCU_Periph, FunctionalState NewState);

#endif /* _DBGMCU_H */
