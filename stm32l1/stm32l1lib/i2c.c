#include <board.h>
#include <i2c.h>

#define I2C_CCR_CALCULATION(__PCLK__, __SPEED__, __COEFF__)     (((((__PCLK__) - 1U)/((__SPEED__) * (__COEFF__))) + 1U) & I2C_CCR_CCR)

#define I2C_RISE_TIME(__FREQRANGE__, __SPEED__)            (((__SPEED__) <= 100000U) ? ((__FREQRANGE__) + 1U) : ((((__FREQRANGE__) * 300U) / 1000U) + 1U))

#define I2C_SPEED_STANDARD(__PCLK__, __SPEED__)            ((I2C_CCR_CALCULATION((__PCLK__), (__SPEED__), 2U) < 4U)? 4U:I2C_CCR_CALCULATION((__PCLK__), (__SPEED__), 2U))
#define I2C_SPEED_FAST(__PCLK__, __SPEED__, __DUTYCYCLE__) (((__DUTYCYCLE__) == I2C_DUTYCYCLE_2)? I2C_CCR_CALCULATION((__PCLK__), (__SPEED__), 3U) : (I2C_CCR_CALCULATION((__PCLK__), (__SPEED__), 25U) | I2C_DUTYCYCLE_16_9))
#define I2C_SPEED(__PCLK__, __SPEED__, __DUTYCYCLE__)      (((__SPEED__) <= 100000U)? (I2C_SPEED_STANDARD((__PCLK__), (__SPEED__))) : \
                                                                  ((I2C_SPEED_FAST((__PCLK__), (__SPEED__), (__DUTYCYCLE__)) & I2C_CCR_CCR) == 0U)? 1U : \
                                                                  ((I2C_SPEED_FAST((__PCLK__), (__SPEED__), (__DUTYCYCLE__))) | I2C_CCR_FS))

#define I2C_7BIT_ADD_WRITE(__ADDRESS__)                    ((unsigned char)((__ADDRESS__) & (unsigned char)(~I2C_OAR1_ADD0)))
#define I2C_7BIT_ADD_READ(__ADDRESS__)                     ((unsigned char)((__ADDRESS__) | I2C_OAR1_ADD0))

#define I2C_10BIT_ADDRESS(__ADDRESS__)                     ((unsigned char)((uint16_t)((__ADDRESS__) & (uint16_t)0x00FF)))
#define I2C_10BIT_HEADER_WRITE(__ADDRESS__)                ((unsigned char)((uint16_t)((uint16_t)(((uint16_t)((__ADDRESS__) & (uint16_t)0x0300)) >> 7) | (uint16_t)0x00F0)))
#define I2C_10BIT_HEADER_READ(__ADDRESS__)                 ((unsigned char)((uint16_t)((uint16_t)(((uint16_t)((__ADDRESS__) & (uint16_t)0x0300)) >> 7) | (uint16_t)(0x00F1))))

#define _I2C_CLEAR_ADDRFLAG(__INSTANCE__)    \
  do{                                        \
    __IO uint32_t tmpreg;                    \
    tmpreg = (__INSTANCE__)->SR1;            \
    tmpreg = (__INSTANCE__)->SR2;            \
  } while(0)

#define I2C_MEM_ADD_MSB(__ADDRESS__)                       ((__ADDRESS__) >> 8U)
#define I2C_MEM_ADD_LSB(__ADDRESS__)                       ((__ADDRESS__) & 0x00FFU)

#define I2C_FLAG_BUSY                   2U
#define I2C_FLAG_SB                     1U
#define I2C_FLAG_ADD10                  8U
#define I2C_FLAG_ADDR                   2U
#define I2C_FLAG_AF                     0x400U
#define I2C_FLAG_TXE                    0x80U
#define I2C_FLAG_BTF                    4U
#define I2C_FLAG_RXNE                   0x40U
#define I2C_FLAG_STOPF                  0x10U

void I2C_Init(I2C_TypeDef *instance, I2C_InitTypeDef *init)
{
  unsigned int freqrange = PCLK1_FREQ / 1000000;
  
  /*---------------------------- I2Cx CR2 Configuration ----------------------*/
  /* Configure I2Cx: Frequency range */
  MODIFY_REG(instance->CR2, I2C_CR2_FREQ, freqrange);

  /*---------------------------- I2Cx TRISE Configuration --------------------*/
  /* Configure I2Cx: Rise Time */
  MODIFY_REG(instance->TRISE, I2C_TRISE_TRISE, I2C_RISE_TIME(freqrange, init->ClockSpeed));

  /*---------------------------- I2Cx CCR Configuration ----------------------*/
  /* Configure I2Cx: Speed */
  MODIFY_REG(instance->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), I2C_SPEED(PCLK1_FREQ, init->ClockSpeed, init->DutyCycle));

  /*---------------------------- I2Cx CR1 Configuration ----------------------*/
  /* Configure I2Cx: Generalcall and NoStretch mode */
  MODIFY_REG(instance->CR1, (I2C_CR1_ENGC | I2C_CR1_NOSTRETCH), (init->GeneralCallMode | init->NoStretchMode));

  /*---------------------------- I2Cx OAR1 Configuration ---------------------*/
  /* Configure I2Cx: Own Address1 and addressing mode */
  MODIFY_REG(instance->OAR1, (I2C_OAR1_ADDMODE | I2C_OAR1_ADD8_9 | I2C_OAR1_ADD1_7 | I2C_OAR1_ADD0), (init->AddressingMode | init->OwnAddress1));

  /*---------------------------- I2Cx OAR2 Configuration ---------------------*/
  /* Configure I2Cx: Dual mode and Own Address2 */
  MODIFY_REG(instance->OAR2, (I2C_OAR2_ENDUAL | I2C_OAR2_ADD2), (init->DualAddressMode | init->OwnAddress2));

  /* Enable the selected I2C peripheral */
  instance->CR1 |= I2C_CR1_PE;
}

static unsigned int I2C_WaitOnSR2FlagUntilTimeout(I2C_TypeDef *instance, unsigned int flag, unsigned int status, unsigned int timeout)
{
  while (--timeout && (instance->SR2 & flag) != status)
    ;
  return timeout;
}

static unsigned int I2C_WaitOnSR1FlagUntilTimeout(I2C_TypeDef *instance, unsigned int flag, unsigned int status, unsigned int timeout)
{
  while (--timeout && (instance->SR1 & flag) != status)
    ;
  return timeout;
}

static unsigned int I2C_WaitOnMasterAddressFlagUntilTimeout(I2C_TypeDef *instance, unsigned int Flag, unsigned int Timeout)
{
  while (--Timeout && !(instance->SR1 & Flag))
  {
    if (instance->SR1 & I2C_FLAG_AF)
    {
      /* Generate Stop */
      SET_BIT(instance->CR1, I2C_CR1_STOP);

      /* Clear AF Flag */
      instance->SR1 &= ~I2C_FLAG_AF;

      return 0;
    }
  }

  return Timeout;
}

static unsigned int I2C_MasterRequestWrite(I2C_TypeDef *instance, unsigned short address, unsigned int timeout)
{
  /* Generate Start */
  SET_BIT(instance->CR1, I2C_CR1_START);

  /* Wait until SB flag is set */
  if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_SB, I2C_FLAG_SB, timeout))
  {
    return 0;
  }

  if (instance->OAR1 & I2C_OAR1_ADDMODE) // 10 bit addressing mode
  {
    /* Send header of slave address */
    instance->DR = I2C_10BIT_HEADER_WRITE(address);

    /* Wait until ADD10 flag is set */
    if (!I2C_WaitOnMasterAddressFlagUntilTimeout(instance, I2C_FLAG_ADD10, timeout))
    {
      return 0;
    }

    /* Send slave address */
    instance->DR = I2C_10BIT_ADDRESS(address);
  }
  else
  {
    /* Send slave address */
    instance->DR = I2C_7BIT_ADD_WRITE(address);
  }

  /* Wait until ADDR flag is set */
  if (!I2C_WaitOnMasterAddressFlagUntilTimeout(instance, I2C_FLAG_ADDR, timeout))
  {
    return 0;
  }

  return 1;
}

static unsigned int I2C_IsAcknowledgeFailed(I2C_TypeDef *instance)
{
  if (instance->SR1 & I2C_FLAG_AF)
  {
    /* Clear NACKF Flag */
    instance->SR1 &= ~I2C_FLAG_AF;

    return 0;
  }

  return 1;
}

static unsigned int I2C_WaitOnTXEFlagUntilTimeout(I2C_TypeDef *instance, unsigned int Timeout)
{
  while (--Timeout && !(instance->SR1 & I2C_FLAG_TXE))
  {
    /* Check if a NACK is detected */
    if (!I2C_IsAcknowledgeFailed(instance))
    {
      return 0;
    }
  }

  return Timeout;
}

unsigned int I2C_Master_Transmit(I2C_TypeDef *instance, unsigned short DevAddress, unsigned char *pData, unsigned int Size, unsigned int Timeout)
{
  /* Wait until BUSY flag is reset */
  if (!I2C_WaitOnSR2FlagUntilTimeout(instance, I2C_FLAG_BUSY, 0, Timeout))
  {
    return 0;
  }

  /* Disable Pos */
  CLEAR_BIT(instance->CR1, I2C_CR1_POS);

  /* Send Slave Address */
  if (!I2C_MasterRequestWrite(instance, DevAddress, Timeout))
  {
    return 0;
  }

  /* Clear ADDR flag */
  _I2C_CLEAR_ADDRFLAG(instance);

  while (Size--)
  {
    /* Wait until TXE flag is set */
    if (!I2C_WaitOnTXEFlagUntilTimeout(instance, Timeout))
    {
        /* Generate Stop */
        SET_BIT(instance->CR1, I2C_CR1_STOP);
        return 0;
    }

    /* Write data to DR */
    instance->DR = *pData++;

    if ((instance->SR1 & I2C_FLAG_BTF) && Size)
    {
      instance->DR = *pData++;
      Size--;
    }

    /* Wait until BTF flag is set */
    if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_BTF, I2C_FLAG_BTF, Timeout))
    {
      /* Generate Stop */
      SET_BIT(instance->CR1, I2C_CR1_STOP);
      return 0;
    }
  }

  /* Generate Stop */
  SET_BIT(instance->CR1, I2C_CR1_STOP);

  return 1;
}

static unsigned int I2C_MasterRequestRead(I2C_TypeDef *instance, unsigned short address, unsigned int timeout)
{
  /* Enable Acknowledge */
  SET_BIT(instance->CR1, I2C_CR1_ACK);

  /* Generate Start */
  SET_BIT(instance->CR1, I2C_CR1_START);

  /* Wait until SB flag is set */
  if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_SB, I2C_FLAG_SB, timeout))
  {
    return 0;
  }

  if (instance->OAR1 & I2C_OAR1_ADDMODE) // 10 bit addressing mode
  {
    /* Send header of slave address */
    instance->DR = I2C_10BIT_HEADER_WRITE(address);

    /* Wait until ADD10 flag is set */
    if (!I2C_WaitOnMasterAddressFlagUntilTimeout(instance, I2C_FLAG_ADD10, timeout))
    {
      return 0;
    }

    /* Send slave address */
    instance->DR = I2C_10BIT_ADDRESS(address);

    /* Wait until ADDR flag is set */
    if (!I2C_WaitOnMasterAddressFlagUntilTimeout(instance, I2C_FLAG_ADDR, timeout))
    {
      return 0;
    }

    /* Clear ADDR flag */
    _I2C_CLEAR_ADDRFLAG(instance);

    SET_BIT(instance->CR1, I2C_CR1_START);

    /* Wait until SB flag is set */
    if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_SB, I2C_FLAG_SB, timeout))
    {
      return 0;
    }

    /* Send header of slave address */
    instance->DR = I2C_10BIT_HEADER_READ(address);
  }
  else
  {
    /* Send slave address */
    instance->DR = I2C_7BIT_ADD_READ(address);
  }

  /* Wait until ADDR flag is set */
  if (!I2C_WaitOnMasterAddressFlagUntilTimeout(instance, I2C_FLAG_ADDR, timeout))
  {
    return 0;
  }

  return 1;
}

static unsigned int I2C_WaitOnRXNEFlagUntilTimeout(I2C_TypeDef *instance, unsigned int Timeout)
{
  while (--Timeout && !(instance->SR1 & I2C_FLAG_RXNE))
  {
    if (instance->SR1 & I2C_FLAG_STOPF)
    {
      /* Clear STOP Flag */
      instance->SR1 &= ~I2C_FLAG_STOPF;

      return 0;
    }
  }

  return Timeout;
}

static unsigned int I2C_Receive_Start(I2C_TypeDef *instance, unsigned int Timeout)
{
  /* Wait until BUSY flag is reset */
  if (!I2C_WaitOnSR2FlagUntilTimeout(instance, I2C_FLAG_BUSY, 0, Timeout))
  {
    return 0;
  }

  /* Disable Pos */
  CLEAR_BIT(instance->CR1, I2C_CR1_POS);

  return 1;
}

static unsigned int I2C_Receive_End(I2C_TypeDef *instance, unsigned char *pData, int Size, unsigned int Timeout)
{
  if (Size == 0)
  {
    /* Clear ADDR flag */
    _I2C_CLEAR_ADDRFLAG(instance);

    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
  }
  else if (Size == 1)
  {
    /* Disable Acknowledge */
    CLEAR_BIT(instance->CR1, I2C_CR1_ACK);

    /* Clear ADDR flag */
    _I2C_CLEAR_ADDRFLAG(instance);

    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
  }
  else if (Size == 2)
  {
    /* Disable Acknowledge */
    CLEAR_BIT(instance->CR1, I2C_CR1_ACK);

    /* Enable Pos */
    SET_BIT(instance->CR1, I2C_CR1_POS);

    /* Clear ADDR flag */
    _I2C_CLEAR_ADDRFLAG(instance);
  }
  else
  {
    /* Enable Acknowledge */
    SET_BIT(instance->CR1, I2C_CR1_ACK);

    /* Clear ADDR flag */
    _I2C_CLEAR_ADDRFLAG(instance);
  }

  while (Size > 0)
  {
    if (Size <= 3)
    {
      /* One byte */
      if (Size == 1)
      {
        /* Wait until RXNE flag is set */
        if (!I2C_WaitOnRXNEFlagUntilTimeout(instance, Timeout))
        {
          return 0;
        }

        /* Read data from DR */
        *pData = (unsigned char)instance->DR;
        /* Update counter */
        Size--;
      }
        /* Two bytes */
      else if (Size == 2)
      {
        /* Wait until BTF flag is set */
        if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_BTF, I2C_FLAG_BTF, Timeout))
        {
          return 0;
        }

        /* Generate Stop */
        SET_BIT(instance->CR1, I2C_CR1_STOP);

        /* Read data from DR */
        *pData++ = (unsigned char)instance->DR;

        /* Update counter */
        Size--;

        /* Read data from DR */
        *pData = (unsigned char)instance->DR;

        /* Update counter */
        Size--;
      }
        /* 3 Last bytes */
      else
      {
        /* Wait until BTF flag is set */
        if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_BTF, I2C_FLAG_BTF, Timeout))
        {
          return 0;
        }

        /* Disable Acknowledge */
        CLEAR_BIT(instance->CR1, I2C_CR1_ACK);

        /* Read data from DR */
        *pData++ = (unsigned char)instance->DR;

        /* Update counter */
        Size--;

        /* Wait until BTF flag is set */
        if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_BTF, I2C_FLAG_BTF, Timeout))
        {
          return 0;
        }

        /* Generate Stop */
        SET_BIT(instance->CR1, I2C_CR1_STOP);

        /* Read data from DR */
        *pData++ = (unsigned char)instance->DR;

        /* Update counter */
        Size--;

        /* Read data from DR */
        *pData = (unsigned char)instance->DR;

        /* Update counter */
        Size--;
      }
    }
    else
    {
      /* Wait until RXNE flag is set */
      if (!I2C_WaitOnRXNEFlagUntilTimeout(instance, Timeout))
      {
        return 0;
      }

      /* Read data from DR */
      *pData++ = (unsigned char)instance->DR;

      /* Update counter */
      Size--;

      if (instance->SR1 & I2C_FLAG_BTF)
      {
        /* Read data from DR */
        *pData++ = (unsigned char)instance->DR;

        /* Update counter */
        Size--;
      }
    }
  }

  return 1;
}

unsigned int I2C_Master_Receive(I2C_TypeDef *instance, unsigned short DevAddress, unsigned char *pData, int Size, unsigned int Timeout)
{
  if (!I2C_Receive_Start(instance, Timeout))
  {
    return 0;
  }

  /* Send Slave Address */
  if (!I2C_MasterRequestRead(instance, DevAddress, Timeout))
  {
    return 0;
  }

  return I2C_Receive_End(instance, pData, Size, Timeout);
}

static unsigned int I2C_RequestMemoryRead(I2C_TypeDef *instance, unsigned short DevAddress, unsigned short MemAddress, unsigned int MemAddSize, unsigned int Timeout)
{
  /* Enable Acknowledge */
  SET_BIT(instance->CR1, I2C_CR1_ACK);

  /* Generate Start */
  SET_BIT(instance->CR1, I2C_CR1_START);

  /* Wait until SB flag is set */
  if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_SB, I2C_FLAG_SB, Timeout))
  {
    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
    return 0;
  }

  /* Send slave address */
  instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);

  /* Wait until ADDR flag is set */
  if (!I2C_WaitOnMasterAddressFlagUntilTimeout(instance, I2C_FLAG_ADDR, Timeout))
  {
    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
    return 0;
  }

  /* Clear ADDR flag */
  _I2C_CLEAR_ADDRFLAG(instance);

  /* Wait until TXE flag is set */
  if (!I2C_WaitOnTXEFlagUntilTimeout(instance, Timeout))
  {
    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
    return 0;
  }

  /* If Memory address size is 8Bit */
  if (MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    instance->DR = I2C_MEM_ADD_LSB(MemAddress);
  }
    /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    instance->DR = I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXE flag is set */
    if (!I2C_WaitOnTXEFlagUntilTimeout(instance, Timeout))
    {
      /* Generate Stop */
      SET_BIT(instance->CR1, I2C_CR1_STOP);
      return 0;
    }

    /* Send LSB of Memory Address */
    instance->DR = I2C_MEM_ADD_LSB(MemAddress);
  }

  /* Wait until TXE flag is set */
  if (!I2C_WaitOnTXEFlagUntilTimeout(instance, Timeout))
  {
    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
    return 0;
  }

  /* Generate Restart */
  SET_BIT(instance->CR1, I2C_CR1_START);

  /* Wait until SB flag is set */
  if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_SB, I2C_FLAG_SB, Timeout))
  {
    return 0;
  }

  /* Send slave address */
  instance->DR = I2C_7BIT_ADD_READ(DevAddress);

  /* Wait until ADDR flag is set */
  return I2C_WaitOnMasterAddressFlagUntilTimeout(instance, I2C_FLAG_ADDR, Timeout);
}

static unsigned int I2C_RequestMemoryWrite(I2C_TypeDef *instance, unsigned short DevAddress, unsigned short MemAddress, unsigned int MemAddSize, unsigned int Timeout)
{
  /* Generate Start */
  SET_BIT(instance->CR1, I2C_CR1_START);

  /* Wait until SB flag is set */
  if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_SB, I2C_FLAG_SB, Timeout))
  {
    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
    return 0;
  }

  /* Send slave address */
  instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);

  /* Wait until ADDR flag is set */
  if (!I2C_WaitOnMasterAddressFlagUntilTimeout(instance, I2C_FLAG_ADDR, Timeout))
  {
    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
    return 0;
  }

  /* Clear ADDR flag */
  _I2C_CLEAR_ADDRFLAG(instance);

  /* Wait until TXE flag is set */
  if (!I2C_WaitOnTXEFlagUntilTimeout(instance, Timeout))
  {
    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
    return 0;
  }

  /* If Memory address size is 8Bit */
  if (MemAddSize == I2C_MEMADD_SIZE_8BIT)
  {
    /* Send Memory Address */
    instance->DR = I2C_MEM_ADD_LSB(MemAddress);
  }
    /* If Memory address size is 16Bit */
  else
  {
    /* Send MSB of Memory Address */
    instance->DR = I2C_MEM_ADD_MSB(MemAddress);

    /* Wait until TXE flag is set */
    if (!I2C_WaitOnTXEFlagUntilTimeout(instance, Timeout))
    {
      /* Generate Stop */
      SET_BIT(instance->CR1, I2C_CR1_STOP);
      return 0;
    }

    /* Send LSB of Memory Address */
    instance->DR = I2C_MEM_ADD_LSB(MemAddress);
  }

  return 1;
}

unsigned int I2C_Mem_Read(I2C_TypeDef *instance, unsigned short DevAddress, unsigned short MemAddress, unsigned int MemAddSize, unsigned char *pData, int Size,
                          unsigned int Timeout)
{
  if (!I2C_Receive_Start(instance, Timeout))
  {
    return  0;
  }

  /* Send Slave Address and Memory Address */
  if (!I2C_RequestMemoryRead(instance, DevAddress, MemAddress, MemAddSize, Timeout))
  {
    return 0;
  }

  return I2C_Receive_End(instance, pData, Size, Timeout);
}

unsigned int I2C_Mem_Write(I2C_TypeDef *instance, unsigned short DevAddress, unsigned short MemAddress, unsigned int MemAddSize, unsigned char *pData, int Size,
                           unsigned int Timeout)
{
  if (!I2C_Receive_Start(instance, Timeout))
  {
    return  0;
  }

  /* Send Slave Address and Memory Address */
  if (!I2C_RequestMemoryWrite(instance, DevAddress, MemAddress, MemAddSize, Timeout))
  {
    return 0;
  }

  while (Size--)
  {
    /* Wait until TXE flag is set */
    if (!I2C_WaitOnTXEFlagUntilTimeout(instance, Timeout))
    {
      /* Generate Stop */
      SET_BIT(instance->CR1, I2C_CR1_STOP);
      return 0;
    }

    /* Write data to DR */
    instance->DR = *pData++;

    if ((instance->SR1 & I2C_FLAG_BTF) && Size)
    {
      instance->DR = *pData++;
      Size--;
    }
  }

  /* Wait until BTF flag is set */
  if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_BTF, I2C_FLAG_BTF, Timeout))
  {
    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
    return 0;
  }

  /* Generate Stop */
  SET_BIT(instance->CR1, I2C_CR1_STOP);

  return 1;
}

unsigned int I2C_Test(I2C_TypeDef *instance, unsigned short DevAddress, unsigned int Timeout)
{
  /* Enable Acknowledge */
  SET_BIT(instance->CR1, I2C_CR1_ACK);

  /* Generate Start */
  SET_BIT(instance->CR1, I2C_CR1_START);

  /* Wait until SB flag is set */
  if (!I2C_WaitOnSR1FlagUntilTimeout(instance, I2C_FLAG_SB, I2C_FLAG_SB, Timeout))
  {
    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
    return 0;
  }

  /* Send slave address */
  instance->DR = I2C_7BIT_ADD_WRITE(DevAddress);

  /* Wait until ADDR flag is set */
  if (!I2C_WaitOnMasterAddressFlagUntilTimeout(instance, I2C_FLAG_ADDR, Timeout))
  {
    /* Generate Stop */
    SET_BIT(instance->CR1, I2C_CR1_STOP);
    return 0;
  }

  /* Generate Stop */
  SET_BIT(instance->CR1, I2C_CR1_STOP);
  return 1;
}
