#ifndef _SPI_H
#define _SPI_H

#include <board.h>

typedef struct
{
  unsigned short SPI_Direction;           /*!< Specifies the SPI unidirectional or bidirectional data mode.
                                         This parameter can be a value of @ref SPI_data_direction */

  unsigned short SPI_Mode;                /*!< Specifies the SPI operating mode.
                                         This parameter can be a value of @ref SPI_mode */

  unsigned short SPI_DataSize;            /*!< Specifies the SPI data size.
                                         This parameter can be a value of @ref SPI_data_size */

  unsigned short SPI_CPOL;                /*!< Specifies the serial clock steady state.
                                         This parameter can be a value of @ref SPI_Clock_Polarity */

  unsigned short SPI_CPHA;                /*!< Specifies the clock active edge for the bit capture.
                                         This parameter can be a value of @ref SPI_Clock_Phase */

  unsigned short SPI_NSS;                 /*!< Specifies whether the NSS signal is managed by
                                         hardware (NSS pin) or by software using the SSI bit.
                                         This parameter can be a value of @ref SPI_Slave_Select_management */
 
  unsigned short SPI_BaudRatePrescaler;   /*!< Specifies the Baud Rate prescaler value which will be
                                         used to configure the transmit and receive SCK clock.
                                         This parameter can be a value of @ref SPI_BaudRate_Prescaler.
                                         @note The communication clock is derived from the master
                                               clock. The slave clock does not need to be set. */

  unsigned short SPI_FirstBit;            /*!< Specifies whether data transfers start from MSB or LSB bit.
                                         This parameter can be a value of @ref SPI_MSB_LSB_transmission */

  unsigned short SPI_CRCPolynomial;       /*!< Specifies the polynomial used for the CRC calculation. */
}SPI_InitTypeDef;

#define SPI_Direction_2Lines_FullDuplex ((unsigned short)0x0000)
#define SPI_Direction_2Lines_RxOnly     ((unsigned short)0x0400)
#define SPI_Direction_1Line_Rx          ((unsigned short)0x8000)
#define SPI_Direction_1Line_Tx          ((unsigned short)0xC000)

#define SPI_Mode_Master                 ((unsigned short)0x0104)
#define SPI_Mode_Slave                  ((unsigned short)0x0000)

#define SPI_DataSize_16b                ((unsigned short)0x0800)
#define SPI_DataSize_8b                 ((unsigned short)0x0000)

#define SPI_CPOL_Low                    ((unsigned short)0x0000)
#define SPI_CPOL_High                   ((unsigned short)0x0002)

#define SPI_CPHA_1Edge                  ((unsigned short)0x0000)
#define SPI_CPHA_2Edge                  ((unsigned short)0x0001)

#define SPI_NSS_Soft                    ((unsigned short)0x0200)
#define SPI_NSS_Hard                    ((unsigned short)0x0000)

#define SPI_BaudRatePrescaler_2         ((unsigned short)0x0000)
#define SPI_BaudRatePrescaler_4         ((unsigned short)0x0008)
#define SPI_BaudRatePrescaler_8         ((unsigned short)0x0010)
#define SPI_BaudRatePrescaler_16        ((unsigned short)0x0018)
#define SPI_BaudRatePrescaler_32        ((unsigned short)0x0020)
#define SPI_BaudRatePrescaler_64        ((unsigned short)0x0028)
#define SPI_BaudRatePrescaler_128       ((unsigned short)0x0030)
#define SPI_BaudRatePrescaler_256       ((unsigned short)0x0038)

#define SPI_FirstBit_MSB                ((unsigned short)0x0000)
#define SPI_FirstBit_LSB                ((unsigned short)0x0080)

#define SPI_I2S_DMAReq_Tx               ((unsigned short)0x0002)
#define SPI_I2S_DMAReq_Rx               ((unsigned short)0x0001)

#define SPI_NSSInternalSoft_Set         ((unsigned short)0x0100)
#define SPI_NSSInternalSoft_Reset       ((unsigned short)0xFEFF)

#define SPI_CRC_Tx                      ((unsigned char)0x00)
#define SPI_CRC_Rx                      ((unsigned char)0x01)

#define SPI_Direction_Rx                ((unsigned short)0xBFFF)
#define SPI_Direction_Tx                ((unsigned short)0x4000)

#define SPI_I2S_IT_TXE                  ((unsigned char)0x71)
#define SPI_I2S_IT_RXNE                 ((unsigned char)0x60)
#define SPI_I2S_IT_ERR                  ((unsigned char)0x50)

#define SPI_I2S_IT_OVR                  ((unsigned char)0x56)
#define SPI_IT_MODF                     ((unsigned char)0x55)
#define SPI_IT_CRCERR                   ((unsigned char)0x54)


#define SPI_I2S_FLAG_RXNE               ((unsigned short)0x0001)
#define SPI_I2S_FLAG_TXE                ((unsigned short)0x0002)
#define I2S_FLAG_CHSIDE                 ((unsigned short)0x0004)
#define I2S_FLAG_UDR                    ((unsigned short)0x0008)
#define SPI_FLAG_CRCERR                 ((unsigned short)0x0010)
#define SPI_FLAG_MODF                   ((unsigned short)0x0020)
#define SPI_I2S_FLAG_OVR                ((unsigned short)0x0040)
#define SPI_I2S_FLAG_BSY                ((unsigned short)0x0080)

#define IS_SPI_CRC_POLYNOMIAL(POLYNOMIAL) ((POLYNOMIAL) >= 0x1)

void SPI_I2S_DeInit(SPI_TypeDef* SPIx);
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, unsigned char SPI_I2S_IT, FunctionalState NewState);
void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, unsigned short SPI_I2S_DMAReq, FunctionalState NewState);
void SPI_I2S_SendData(SPI_TypeDef* SPIx, unsigned short Data);
unsigned short SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, unsigned short SPI_NSSInternalSoft);
void SPI_SSOutputCmd(SPI_TypeDef* SPIx, FunctionalState NewState);
void SPI_DataSizeConfig(SPI_TypeDef* SPIx, unsigned short SPI_DataSize);
void SPI_TransmitCRC(SPI_TypeDef* SPIx);
void SPI_CalculateCRC(SPI_TypeDef* SPIx, FunctionalState NewState);
unsigned short SPI_GetCRC(SPI_TypeDef* SPIx, unsigned char SPI_CRC);
unsigned short SPI_GetCRCPolynomial(SPI_TypeDef* SPIx);
void SPI_BiDirectionalLineConfig(SPI_TypeDef* SPIx, unsigned short SPI_Direction);
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, unsigned short SPI_I2S_FLAG);
void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, unsigned short SPI_I2S_FLAG);
ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, unsigned char SPI_I2S_IT);
void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, unsigned char SPI_I2S_IT);
void SPI_ClockEnable(SPI_TypeDef* SPIx, int enable);

#endif /*_SPI_H */
