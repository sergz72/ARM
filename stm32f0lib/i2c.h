#ifndef __STM32F0XX_I2C_H
#define __STM32F0XX_I2C_H

#include "stm32f0xx.h"

/**
  * @brief  I2C Init structure definition
  */

typedef struct
{
  uint32_t I2C_Timing;              /*!< Specifies the I2C_TIMINGR_register value.
                                         This parameter must be set by referring to I2C_Timing_Config_Tool*/

  uint32_t I2C_AnalogFilter;        /*!< Enables or disables analog noise filter.
                                         This parameter can be a value of @ref I2C_Analog_Filter*/

  uint32_t I2C_DigitalFilter;       /*!< Configures the digital noise filter.
                                         This parameter can be a number between 0x00 and 0x0F*/

  uint32_t I2C_Mode;                /*!< Specifies the I2C mode.
                                         This parameter can be a value of @ref I2C_mode*/

  uint32_t I2C_OwnAddress1;         /*!< Specifies the device own address 1.
                                         This parameter can be a 7-bit or 10-bit address*/

  uint32_t I2C_Ack;                 /*!< Enables or disables the acknowledgement.
                                         This parameter can be a value of @ref I2C_acknowledgement*/

  uint32_t I2C_AcknowledgedAddress; /*!< Specifies if 7-bit or 10-bit address is acknowledged.
                                         This parameter can be a value of @ref I2C_acknowledged_address*/
} I2C_InitTypeDef;

#define I2C_AnalogFilter_Enable         ((uint32_t)0x00000000)
#define I2C_AnalogFilter_Disable        I2C_CR1_ANFOFF

#define I2C_Mode_I2C                    ((uint32_t)0x00000000)
#define I2C_Mode_SMBusDevice            I2C_CR1_SMBDEN
#define I2C_Mode_SMBusHost              I2C_CR1_SMBHEN

#define I2C_Ack_Enable                  ((uint32_t)0x00000000)
#define I2C_Ack_Disable                 I2C_CR2_NACK

#define I2C_AcknowledgedAddress_7bit    ((uint32_t)0x00000000)
#define I2C_AcknowledgedAddress_10bit   I2C_OAR1_OA1MODE

#define I2C_Direction_Transmitter       ((uint16_t)0x0000)
#define I2C_Direction_Receiver          ((uint16_t)0x0400)

#define I2C_DMAReq_Tx                   I2C_CR1_TXDMAEN
#define I2C_DMAReq_Rx                   I2C_CR1_RXDMAEN

#define I2C_OA2_NoMask                  ((uint8_t)0x00)
#define I2C_OA2_Mask01                  ((uint8_t)0x01)                 
#define I2C_OA2_Mask02                  ((uint8_t)0x02)
#define I2C_OA2_Mask03                  ((uint8_t)0x03)
#define I2C_OA2_Mask04                  ((uint8_t)0x04)
#define I2C_OA2_Mask05                  ((uint8_t)0x05)
#define I2C_OA2_Mask06                  ((uint8_t)0x06)
#define I2C_OA2_Mask07                  ((uint8_t)0x07)

#define I2C_Register_CR1                ((uint8_t)0x00)
#define I2C_Register_CR2                ((uint8_t)0x04)
#define I2C_Register_OAR1               ((uint8_t)0x08)
#define I2C_Register_OAR2               ((uint8_t)0x0C)
#define I2C_Register_TIMINGR            ((uint8_t)0x10)
#define I2C_Register_TIMEOUTR           ((uint8_t)0x14)
#define I2C_Register_ISR                ((uint8_t)0x18)
#define I2C_Register_ICR                ((uint8_t)0x1C)
#define I2C_Register_PECR               ((uint8_t)0x20)
#define I2C_Register_RXDR               ((uint8_t)0x24)
#define I2C_Register_TXDR               ((uint8_t)0x28)

#define I2C_IT_ERRI                     I2C_CR1_ERRIE
#define I2C_IT_TCI                      I2C_CR1_TCIE
#define I2C_IT_STOPI                    I2C_CR1_STOPIE
#define I2C_IT_NACKI                    I2C_CR1_NACKIE
#define I2C_IT_ADDRI                    I2C_CR1_ADDRIE
#define I2C_IT_RXI                      I2C_CR1_RXIE
#define I2C_IT_TXI                      I2C_CR1_TXIE

#define  I2C_FLAG_TXE                   I2C_ISR_TXE
#define  I2C_FLAG_TXIS                  I2C_ISR_TXIS
#define  I2C_FLAG_RXNE                  I2C_ISR_RXNE
#define  I2C_FLAG_ADDR                  I2C_ISR_ADDR
#define  I2C_FLAG_NACKF                 I2C_ISR_NACKF
#define  I2C_FLAG_STOPF                 I2C_ISR_STOPF
#define  I2C_FLAG_TC                    I2C_ISR_TC
#define  I2C_FLAG_TCR                   I2C_ISR_TCR
#define  I2C_FLAG_BERR                  I2C_ISR_BERR
#define  I2C_FLAG_ARLO                  I2C_ISR_ARLO
#define  I2C_FLAG_OVR                   I2C_ISR_OVR
#define  I2C_FLAG_PECERR                I2C_ISR_PECERR
#define  I2C_FLAG_TIMEOUT               I2C_ISR_TIMEOUT
#define  I2C_FLAG_ALERT                 I2C_ISR_ALERT
#define  I2C_FLAG_BUSY                  I2C_ISR_BUSY

#define  I2C_IT_TXIS                    I2C_ISR_TXIS
#define  I2C_IT_RXNE                    I2C_ISR_RXNE
#define  I2C_IT_ADDR                    I2C_ISR_ADDR
#define  I2C_IT_NACKF                   I2C_ISR_NACKF
#define  I2C_IT_STOPF                   I2C_ISR_STOPF
#define  I2C_IT_TC                      I2C_ISR_TC
#define  I2C_IT_TCR                     I2C_ISR_TCR
#define  I2C_IT_BERR                    I2C_ISR_BERR
#define  I2C_IT_ARLO                    I2C_ISR_ARLO
#define  I2C_IT_OVR                     I2C_ISR_OVR
#define  I2C_IT_PECERR                  I2C_ISR_PECERR
#define  I2C_IT_TIMEOUT                 I2C_ISR_TIMEOUT
#define  I2C_IT_ALERT                   I2C_ISR_ALERT

#define  I2C_Reload_Mode                I2C_CR2_RELOAD
#define  I2C_AutoEnd_Mode               I2C_CR2_AUTOEND
#define  I2C_SoftEnd_Mode               ((uint32_t)0x00000000)

#define  I2C_No_StartStop                 ((uint32_t)0x00000000)
#define  I2C_Generate_Stop                I2C_CR2_STOP
#define  I2C_Generate_Start_Read          (uint32_t)(I2C_CR2_START | I2C_CR2_RD_WRN)
#define  I2C_Generate_Start_Write         I2C_CR2_START

/* Initialization and Configuration functions *********************************/
void I2C_DeInit(I2C_TypeDef* I2Cx);
void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct);
void I2C_StructInit(I2C_InitTypeDef* I2C_InitStruct);
void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_SoftwareResetCmd(I2C_TypeDef* I2Cx);
void I2C_ITConfig(I2C_TypeDef* I2Cx, uint32_t I2C_IT, FunctionalState NewState);
void I2C_StretchClockCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_StopModeCmd(I2C_TypeDef* I2Cx, FunctionalState NewState); /*!< not applicable for STM32F030 devices */
void I2C_DualAddressCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_OwnAddress2Config(I2C_TypeDef* I2Cx, uint16_t Address, uint8_t Mask);
void I2C_GeneralCallCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_SlaveByteControlCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_SlaveAddressConfig(I2C_TypeDef* I2Cx, uint16_t Address);
void I2C_10BitAddressingModeCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);

/* Communications handling functions ******************************************/
void I2C_AutoEndCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_ReloadCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_NumberOfBytesConfig(I2C_TypeDef* I2Cx, uint8_t Number_Bytes);
void I2C_MasterRequestConfig(I2C_TypeDef* I2Cx, uint16_t I2C_Direction);
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_10BitAddressHeaderCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState);
uint8_t I2C_GetAddressMatched(I2C_TypeDef* I2Cx);
uint16_t I2C_GetTransferDirection(I2C_TypeDef* I2Cx);
void I2C_TransferHandling(I2C_TypeDef* I2Cx, uint16_t Address, uint8_t Number_Bytes, uint32_t ReloadEndMode, uint32_t StartStopMode);

/*  SMBUS management functions ************************************************/
void I2C_SMBusAlertCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_ClockTimeoutCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_ExtendedClockTimeoutCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_IdleClockTimeoutCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_TimeoutAConfig(I2C_TypeDef* I2Cx, uint16_t Timeout);
void I2C_TimeoutBConfig(I2C_TypeDef* I2Cx, uint16_t Timeout);
void I2C_CalculatePEC(I2C_TypeDef* I2Cx, FunctionalState NewState);
void I2C_PECRequestCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
uint8_t I2C_GetPEC(I2C_TypeDef* I2Cx);

/* I2C registers management functions *****************************************/
uint32_t I2C_ReadRegister(I2C_TypeDef* I2Cx, uint8_t I2C_Register);

/* Data transfers management functions ****************************************/
void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);
uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);

/* DMA transfers management functions *****************************************/
void I2C_DMACmd(I2C_TypeDef* I2Cx, uint32_t I2C_DMAReq, FunctionalState NewState);

/* Interrupts and flags management functions **********************************/
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
void I2C_ClearFlag(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
ITStatus I2C_GetITStatus(I2C_TypeDef* I2Cx, uint32_t I2C_IT);
void I2C_ClearITPendingBit(I2C_TypeDef* I2Cx, uint32_t I2C_IT);

#endif /*__STM32F0XX_I2C_H */
