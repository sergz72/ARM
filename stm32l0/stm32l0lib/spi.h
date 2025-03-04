#ifndef _SPI_H
#define _SPI_H

/**
  * @brief  SPI Configuration Structure definition
  */
typedef struct
{
  unsigned int Mode;                /*!< Specifies the SPI operating mode.
                                     This parameter can be a value of @ref SPI_Mode */

  unsigned int Direction;           /*!< Specifies the SPI bidirectional mode state.
                                     This parameter can be a value of @ref SPI_Direction */

  unsigned int DataSize;            /*!< Specifies the SPI data size.
                                     This parameter can be a value of @ref SPI_Data_Size */

  unsigned int CLKPolarity;         /*!< Specifies the serial clock steady state.
                                     This parameter can be a value of @ref SPI_Clock_Polarity */

  unsigned int CLKPhase;            /*!< Specifies the clock active edge for the bit capture.
                                     This parameter can be a value of @ref SPI_Clock_Phase */

  unsigned int NSS;                 /*!< Specifies whether the NSS signal is managed by
                                     hardware (NSS pin) or by software using the SSI bit.
                                     This parameter can be a value of @ref SPI_Slave_Select_management */

  unsigned int BaudRatePrescaler;   /*!< Specifies the Baud Rate prescaler value which will be
                                     used to configure the transmit and receive SCK clock.
                                     This parameter can be a value of @ref SPI_BaudRate_Prescaler
                                     @note The communication clock is derived from the master
                                     clock. The slave clock does not need to be set. */

  unsigned int FirstBit;            /*!< Specifies whether data transfers start from MSB or LSB bit.
                                     This parameter can be a value of @ref SPI_MSB_LSB_transmission */

  unsigned int TIMode;              /*!< Specifies if the TI mode is enabled or not.
                                     This parameter can be a value of @ref SPI_TI_mode */

  unsigned int CRCCalculation;      /*!< Specifies if the CRC calculation is enabled or not.
                                     This parameter can be a value of @ref SPI_CRC_Calculation */

  unsigned int CRCPolynomial;       /*!< Specifies the polynomial used for the CRC calculation.
                                     This parameter must be an odd number between Min_Data = 1 and Max_Data = 65535 */
} SPI_InitTypeDef;

#define SPI_CRCCALCULATION_DISABLE      0U
#define SPI_CRCCALCULATION_ENABLE       SPI_CR1_CRCEN

#define SPI_DIRECTION_2LINES            0U
#define SPI_DIRECTION_2LINES_RXONLY     SPI_CR1_RXONLY
#define SPI_DIRECTION_1LINE             SPI_CR1_BIDIMODE

#define SPI_MODE_SLAVE                  0U
#define SPI_MODE_MASTER                 (SPI_CR1_MSTR | SPI_CR1_SSI)

#define SPI_DATASIZE_8BIT               0U
#define SPI_DATASIZE_16BIT              SPI_CR1_DFF

#define SPI_POLARITY_LOW                0U
#define SPI_POLARITY_HIGH               SPI_CR1_CPOL

#define SPI_PHASE_1EDGE                 0U
#define SPI_PHASE_2EDGE                 SPI_CR1_CPHA

#define SPI_NSS_SOFT                    SPI_CR1_SSM
#define SPI_NSS_HARD_INPUT              0U
#define SPI_NSS_HARD_OUTPUT             (SPI_CR2_SSOE << 16U)

#define SPI_BAUDRATEPRESCALER_2         0U
#define SPI_BAUDRATEPRESCALER_4         (SPI_CR1_BR_0)
#define SPI_BAUDRATEPRESCALER_8         (SPI_CR1_BR_1)
#define SPI_BAUDRATEPRESCALER_16        (SPI_CR1_BR_1 | SPI_CR1_BR_0)
#define SPI_BAUDRATEPRESCALER_32        (SPI_CR1_BR_2)
#define SPI_BAUDRATEPRESCALER_64        (SPI_CR1_BR_2 | SPI_CR1_BR_0)
#define SPI_BAUDRATEPRESCALER_128       (SPI_CR1_BR_2 | SPI_CR1_BR_1)
#define SPI_BAUDRATEPRESCALER_256       (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0)

#define SPI_FIRSTBIT_MSB                0U
#define SPI_FIRSTBIT_LSB                SPI_CR1_LSBFIRST

#define SPI_TIMODE_DISABLE              0U
#define SPI_TIMODE_ENABLE               SPI_CR2_FRF

void SPI_Init(SPI_TypeDef *instance, SPI_InitTypeDef *init);
void SPI_Enable(SPI_TypeDef *instance);
void SPI_Disable(SPI_TypeDef *instance);
unsigned int SPI_TransmitReceive(SPI_TypeDef *instance, unsigned char *txData, unsigned char *rxData, unsigned int size, unsigned int timeout);

#endif
