#ifndef _I2C_H
#define _I2C_H

typedef struct
{
  unsigned int ClockSpeed;       /*!< Specifies the clock frequency.
                                  This parameter must be set to a value lower than 400kHz */
  unsigned int GeneralCallMode;  /*!< Specifies if general call mode is selected.
                                  This parameter can be a value of @ref I2C_general_call_addressing_mode */
  unsigned int AnalogNoiseFilter; // 1 - enabled, o - disabled
  unsigned int DigitalNoiseFilter; // from 0 to 15
} I2C_InitTypeDef;

#define  I2C_Reload_Mode                I2C_CR2_RELOAD
#define  I2C_AutoEnd_Mode               I2C_CR2_AUTOEND
#define  I2C_SoftEnd_Mode               0

#define  I2C_No_StartStop                 0
#define  I2C_Generate_Stop                I2C_CR2_STOP
#define  I2C_Generate_Start_Read          (I2C_CR2_START | I2C_CR2_RD_WRN)
#define  I2C_Generate_Start_Write         I2C_CR2_START

void I2C_Init(I2C_TypeDef *instance, I2C_InitTypeDef *init);
void I2C_Enable(I2C_TypeDef *instance);
void I2C_Disable(I2C_TypeDef *instance);
unsigned int I2C_Test(I2C_TypeDef *instance, unsigned short DevAddress, unsigned int Timeout);
void I2C_TransferHandling(I2C_TypeDef* I2Cx, unsigned short Address, unsigned int Number_Bytes, unsigned int ReloadEndMode, unsigned int StartStopMode);

#endif
