#include <board.h>
#include <i2c.h>

#define TIMING_CLEAR_MASK   (0xF0FFFFFFU)  /*!< I2C TIMING clear register Mask */

#define I2C_FLAG_TXE                    I2C_ISR_TXE
#define I2C_FLAG_TXIS                   I2C_ISR_TXIS
#define I2C_FLAG_RXNE                   I2C_ISR_RXNE
#define I2C_FLAG_ADDR                   I2C_ISR_ADDR
#define I2C_FLAG_AF                     I2C_ISR_NACKF
#define I2C_FLAG_STOPF                  I2C_ISR_STOPF
#define I2C_FLAG_TC                     I2C_ISR_TC
#define I2C_FLAG_TCR                    I2C_ISR_TCR
#define I2C_FLAG_BERR                   I2C_ISR_BERR
#define I2C_FLAG_ARLO                   I2C_ISR_ARLO
#define I2C_FLAG_OVR                    I2C_ISR_OVR
#define I2C_FLAG_PECERR                 I2C_ISR_PECERR
#define I2C_FLAG_TIMEOUT                I2C_ISR_TIMEOUT
#define I2C_FLAG_ALERT                  I2C_ISR_ALERT
#define I2C_FLAG_BUSY                   I2C_ISR_BUSY
#define I2C_FLAG_DIR                    I2C_ISR_DIR

#define I2C_TIMEOUT_BUSY 25 // 25 ms

#define I2C_MEM_ADD_MSB(__ADDRESS__)  (__ADDRESS__ >> 8)
#define I2C_MEM_ADD_LSB(__ADDRESS__)  (__ADDRESS__ & 0xFF)

#define  I2C_NO_STARTSTOP               0U
#define  I2C_GENERATE_STOP              I2C_CR2_STOP
#define  I2C_GENERATE_START_READ        (I2C_CR2_START | I2C_CR2_RD_WRN)
#define  I2C_GENERATE_START_WRITE       I2C_CR2_START

#define I2C_RESET_CR2(__HANDLE__) ((__HANDLE__)->CR2 &= ~((unsigned int)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)))

#define  I2C_RELOAD_MODE                I2C_CR2_RELOAD
#define  I2C_AUTOEND_MODE               I2C_CR2_AUTOEND
#define  I2C_SOFTEND_MODE               0U

/**
  * @brief  Initializes the I2C according to the specified parameters
  *         in the I2C_InitTypeDef and initialize the associated handle.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval HAL status
  */
void I2C_Init(I2C_TypeDef *instance, I2C_InitTypeDef *init)
{
  /* Configure I2Cx: Frequency range */
  instance->TIMINGR = init->Timing & TIMING_CLEAR_MASK;

  /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
  /* Disable Own Address1 before set the Own Address1 configuration */
  instance->OAR1 &= ~I2C_OAR1_OA1EN;

  /* Configure I2Cx: Own Address1 and ack own address1 mode */
  if (init->AddressingMode == I2C_ADDRESSINGMODE_7BIT)
  {
    instance->OAR1 = (I2C_OAR1_OA1EN | init->OwnAddress1);
  }
  else /* I2C_ADDRESSINGMODE_10BIT */
  {
    instance->OAR1 = (I2C_OAR1_OA1EN | I2C_OAR1_OA1MODE | init->OwnAddress1);
    /*---------------------------- I2Cx CR2 Configuration ----------------------*/
    instance->CR2 = (I2C_CR2_ADD10);
  }

  /*---------------------------- I2Cx CR2 Configuration ----------------------*/
  /* Enable the AUTOEND by default, and enable NACK (should be disable only during Slave process */
  instance->CR2 |= (I2C_CR2_AUTOEND | I2C_CR2_NACK);

  /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
  /* Disable Own Address2 before set the Own Address2 configuration */
  instance->OAR2 &= ~I2C_DUALADDRESS_ENABLE;

  /* Configure I2Cx: Dual mode and Own Address2 */
  instance->OAR2 = (init->DualAddressMode | init->OwnAddress2 | (init->OwnAddress2Masks << 8));

  /*---------------------------- I2Cx CR1 Configuration ----------------------*/
  /* Configure I2Cx: Generalcall and NoStretch mode */
  instance->CR1 = (init->GeneralCallMode | init->NoStretchMode);

  /* Enable the selected I2C peripheral */
  instance->CR1 |= I2C_CR1_PE;
}

static unsigned int I2C_WaitOnFlagUntilTimeout(I2C_TypeDef *instance, unsigned int Flag, FlagStatus Status, unsigned int Timeout)
{
  if (Timeout != _MAX_DELAY)
    Timeout *= 10000;
  while (((instance->ISR & Flag) ? SET : RESET) == Status)
  {
    /* Check for the Timeout */
    if(Timeout != _MAX_DELAY)
    {
      if(Timeout-- == 0)
        return 1;
    }
  }
  return 0;
}

static void I2C_Flush_TXDR(I2C_TypeDef *instance)
{
  /* If a pending TXIS flag is set */
  /* Write a dummy data in TXDR to clear it */
  if(instance->ISR & I2C_FLAG_TXIS)
    instance->TXDR = 0x00;

  /* Flush TX register if not empty */
  if (!(instance->ISR & I2C_FLAG_TXE))
    instance->ISR |= I2C_FLAG_TXE;
}

static unsigned int I2C_IsAcknowledgeFailed(I2C_TypeDef *instance, unsigned int Timeout)
{
  if (instance->ISR & I2C_FLAG_AF)
  {
    if (Timeout != _MAX_DELAY)
      Timeout *= 10000;
    /* Wait until STOP Flag is reset */
    /* AutoEnd should be initiate after AF */
    while (!(instance->ISR & I2C_FLAG_STOPF))
    {
      /* Check for the Timeout */
      if(Timeout != _MAX_DELAY)
      {
        if(Timeout-- == 0)
          return 1;
      }
    }

    /* Clear NACKF Flag */
    instance->ICR |= I2C_FLAG_AF;

    /* Clear STOP Flag */
    instance->ICR |= I2C_FLAG_STOPF;

    /* Flush TX register */
    I2C_Flush_TXDR(instance);

    /* Clear Configuration Register 2 */
    I2C_RESET_CR2(instance);

    return 1;
  }
  return 0;
}

static unsigned int I2C_WaitOnTXISFlagUntilTimeout(I2C_TypeDef *instance, unsigned int Timeout)
{
  if (Timeout != _MAX_DELAY)
    Timeout *= 10000;
  while (!(instance->ISR & I2C_FLAG_TXIS))
  {
    /* Check if a NACK is detected */
    if(I2C_IsAcknowledgeFailed(instance, Timeout))
      return 1;

    /* Check for the Timeout */
    if(Timeout != _MAX_DELAY)
    {
      if(Timeout-- == 0)
        return 1;
    }
  }
  return 0;
}

static void I2C_TransferConfig(I2C_TypeDef *instance, unsigned short DevAddress, unsigned char Size, unsigned int Mode, unsigned int Request)
{
  uint32_t tmpreg;

  /* Get the CR2 register value */
  tmpreg = instance->CR2;

  /* clear tmpreg specific bits */
  tmpreg &= ~((unsigned int)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));

  /* update tmpreg */
  tmpreg |= (((unsigned int)DevAddress & I2C_CR2_SADD) | (((unsigned int)Size << 16U) & I2C_CR2_NBYTES) | Mode | Request);

  /* update CR2 register */
  instance->CR2 = tmpreg;
}

static unsigned int I2C_RequestMemoryRead(I2C_TypeDef *instance, unsigned short DevAddress, unsigned short MemAddress, unsigned int MemAddSize,
                                          unsigned int Timeout)
{
  I2C_TransferConfig(instance, DevAddress, MemAddSize, I2C_SOFTEND_MODE, I2C_GENERATE_START_WRITE);

  /* Wait until TXIS flag is set */
  if (I2C_WaitOnTXISFlagUntilTimeout(instance, Timeout))
    return 1;

  /* If Memory address size is 8Bit */
  if(MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }
    /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    instance->TXDR = I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXIS flag is set */
    if(I2C_WaitOnTXISFlagUntilTimeout(instance, Timeout))
      return 1;

    /* Send LSB of Memory Address */
    instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }

  /* Wait until TC flag is set */
  return I2C_WaitOnFlagUntilTimeout(instance, I2C_FLAG_TC, RESET, Timeout);
}

static unsigned int I2C_RequestMemoryWrite(I2C_TypeDef *instance, unsigned short DevAddress, unsigned short MemAddress, unsigned int MemAddSize,
                                           unsigned int Timeout)
{
  I2C_TransferConfig(instance, DevAddress, MemAddSize, I2C_RELOAD_MODE, I2C_GENERATE_START_WRITE);

  /* Wait until TXIS flag is set */
  if (I2C_WaitOnTXISFlagUntilTimeout(instance, Timeout))
    return 1;

  /* If Memory address size is 8Bit */
  if(MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }
    /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    instance->TXDR = I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXIS flag is set */
    if(I2C_WaitOnTXISFlagUntilTimeout(instance, Timeout))
      return 1;

    /* Send LSB of Memory Address */
    instance->TXDR = I2C_MEM_ADD_LSB(MemAddress);
  }

  /* Wait until TC flag is set */
  return I2C_WaitOnFlagUntilTimeout(instance, I2C_FLAG_TCR, RESET, Timeout);
}

static unsigned int I2C_WaitOnSTOPFlagUntilTimeout(I2C_TypeDef *instance, unsigned int Timeout)
{
  if (Timeout != _MAX_DELAY)
    Timeout *= 10000;
  while (!(instance->ISR & I2C_FLAG_STOPF))
  {
    /* Check if a NACK is detected */
    if (I2C_IsAcknowledgeFailed(instance, Timeout))
      return 1;

    /* Check for the Timeout */
    if(Timeout-- == 0)
      return 1;
  }

  return 0;
}

unsigned int I2C_Mem_Read(I2C_TypeDef *instance, unsigned short DevAddress, unsigned short MemAddress, unsigned int MemAddSize, unsigned char *pData,
                          unsigned int Size, unsigned int Timeout)
{
  if (I2C_WaitOnFlagUntilTimeout(instance, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY))
    return 1;

  if (I2C_RequestMemoryRead(instance, DevAddress, MemAddress, MemAddSize, Timeout))
    return 1;

  I2C_TransferConfig(instance, DevAddress, Size, I2C_AUTOEND_MODE, I2C_GENERATE_START_READ);

  while (Size--)
  {
    /* Wait until RXNE flag is set */
    if (I2C_WaitOnFlagUntilTimeout(instance, I2C_FLAG_RXNE, RESET, Timeout))
      return 1;

    /* Read data from RXDR */
    *pData++ = instance->RXDR;
  }

  /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
  /* Wait until STOPF flag is reset */
  if(I2C_WaitOnSTOPFlagUntilTimeout(instance, Timeout))
    return 1;

  /* Clear STOP Flag */
  instance->ICR |= I2C_FLAG_STOPF;

  /* Clear Configuration Register 2 */
  I2C_RESET_CR2(instance);

  return 0;
}

unsigned int I2C_Mem_Write(I2C_TypeDef *instance, unsigned short DevAddress, unsigned short MemAddress, unsigned int MemAddSize, unsigned char *pData,
                           unsigned int Size, unsigned int Timeout)
{
  if (I2C_WaitOnFlagUntilTimeout(instance, I2C_FLAG_BUSY, SET, I2C_TIMEOUT_BUSY))
    return 1;

  if (I2C_RequestMemoryWrite(instance, DevAddress, MemAddress, MemAddSize, Timeout))
    return 1;

  I2C_TransferConfig(instance, DevAddress, Size, I2C_AUTOEND_MODE, I2C_NO_STARTSTOP);

  while (Size--)
  {
    /* Wait until RXNE flag is set */
    if (I2C_WaitOnTXISFlagUntilTimeout(instance, Timeout))
      return 1;

    /* Read data from RXDR */
    instance->TXDR = *pData++;
  }

  /* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
  /* Wait until STOPF flag is reset */
  if(I2C_WaitOnSTOPFlagUntilTimeout(instance, Timeout))
    return 1;

  /* Clear STOP Flag */
  instance->ICR |= I2C_FLAG_STOPF;

  /* Clear Configuration Register 2 */
  I2C_RESET_CR2(instance);

  return 0;
}
