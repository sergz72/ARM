#ifndef _I2C_H
#define _I2C_H

typedef struct
{
  unsigned int ClockSpeed;       /*!< Specifies the clock frequency.
                                  This parameter must be set to a value lower than 400kHz */

  unsigned int DutyCycle;        /*!< Specifies the I2C fast mode duty cycle.
                                  This parameter can be a value of @ref I2C_duty_cycle_in_fast_mode */

  unsigned int OwnAddress1;      /*!< Specifies the first device own address.
                                  This parameter can be a 7-bit or 10-bit address. */

  unsigned int AddressingMode;   /*!< Specifies if 7-bit or 10-bit addressing mode is selected.
                                  This parameter can be a value of @ref I2C_addressing_mode */

  unsigned int DualAddressMode;  /*!< Specifies if dual addressing mode is selected.
                                  This parameter can be a value of @ref I2C_dual_addressing_mode */

  unsigned int OwnAddress2;      /*!< Specifies the second device own address if dual addressing mode is selected
                                  This parameter can be a 7-bit address. */

  unsigned int GeneralCallMode;  /*!< Specifies if general call mode is selected.
                                  This parameter can be a value of @ref I2C_general_call_addressing_mode */

  unsigned int NoStretchMode;    /*!< Specifies if nostretch mode is selected.
                                  This parameter can be a value of @ref I2C_nostretch_mode */

} I2C_InitTypeDef;

#define I2C_ADDRESSINGMODE_7BIT         0x00004000U
#define I2C_ADDRESSINGMODE_10BIT        (I2C_OAR1_ADDMODE | 0x00004000U)

#define I2C_DUALADDRESS_DISABLE        0x00000000U
#define I2C_DUALADDRESS_ENABLE         I2C_OAR2_ENDUAL

#define I2C_GENERALCALL_DISABLE        0x00000000U
#define I2C_GENERALCALL_ENABLE         I2C_CR1_ENGC

#define I2C_NOSTRETCH_DISABLE          0x00000000U
#define I2C_NOSTRETCH_ENABLE           I2C_CR1_NOSTRETCH

#define I2C_DUTYCYCLE_2                 0x00000000U
#define I2C_DUTYCYCLE_16_9              I2C_CCR_DUTY

#define I2C_MEMADD_SIZE_8BIT            0x00000001U
#define I2C_MEMADD_SIZE_16BIT           0x00000010U

void I2C_Init(I2C_TypeDef *instance, I2C_InitTypeDef *init);
unsigned int I2C_Master_Transmit(I2C_TypeDef *instance, unsigned short DevAddress, unsigned char *pData, unsigned int Size, unsigned int Timeout);
unsigned int I2C_Master_Receive(I2C_TypeDef *instance, unsigned short DevAddress, unsigned char *pData, int Size, unsigned int Timeout);
unsigned int I2C_Mem_Read(I2C_TypeDef *instance, unsigned short DevAddress, unsigned short MemAddress, unsigned int MemAddSize, unsigned char *pData, int Size,
                          unsigned int Timeout);
unsigned int I2C_Mem_Write(I2C_TypeDef *instance, unsigned short DevAddress, unsigned short MemAddress, unsigned int MemAddSize, unsigned char *pData, int Size,
                           unsigned int Timeout);
unsigned int I2C_Test(I2C_TypeDef *instance, unsigned short DevAddress, unsigned int Timeout);

#endif
