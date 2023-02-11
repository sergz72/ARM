#ifndef __STM32F0XX_I2C_H
#define __STM32F0XX_I2C_H

#include <stm32f0xx.h>

typedef struct
{
  int GeneralCallEnable;
  int ClockStretchingEnable;
  int SlaveByteControlEnable;
  int RXDmaEnable;
  int TXDmaEnable;
  int AnalogFilterEnable;
  unsigned int DigitalFilter;
  int ErrorInterruptsEnable;
  int StopInterruptEnable;
  int NAckInterruptEnable;
  int AddressInterruptEnable;
  unsigned int OwnAddress1;
  unsigned int OwnAddress2;
  unsigned int OwnAddress2Mask;
  unsigned int Timing;
  void (*TXISHandler)(I2C_TypeDef* I2Cx);
  void (*RXNEHandler)(I2C_TypeDef* I2Cx);
  void (*ADDRHandler)(I2C_TypeDef* I2Cx);
  void (*NACKHandler)(I2C_TypeDef* I2Cx);
  void (*STOPHandler)(I2C_TypeDef* I2Cx);
  void (*TCHandler)(I2C_TypeDef* I2Cx);
  void (*TCRHandler)(I2C_TypeDef* I2Cx);
  void (*BERRHandler)(I2C_TypeDef* I2Cx);
  void (*ARLOHandler)(I2C_TypeDef* I2Cx);
  void (*OVRHandler)(I2C_TypeDef* I2Cx);
  void (*PECERRHandler)(I2C_TypeDef* I2Cx);
  void (*TimeoutHandler)(I2C_TypeDef* I2Cx);
  void (*AlertHandler)(I2C_TypeDef* I2Cx);
} I2C_InitTypeDef;

#define I2C_DigitalFilter_Disable 0

void I2C_Init(I2C_TypeDef* I2Cx, const I2C_InitTypeDef* I2C_InitStruct);

#endif /*__STM32F0XX_I2C_H */
