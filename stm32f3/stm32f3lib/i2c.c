#include <board.h>
#include <i2c.h>

static int WaitOnFlagUntilTimeout(I2C_TypeDef *i2c, unsigned int flag, unsigned int state, unsigned int timeout)
{
  while ((i2c->ISR & flag) != state)
  {
    timeout--;
    if (!timeout)
      return I2C_RCTIMEOUT;
  }

  return I2C_RCOK;
}

static int i2c_send(I2C_TypeDef *i2c, unsigned char *data, unsigned int count, unsigned int timeout)
{
  int rc;

  while (count--)
  {
    while (!(i2c->ISR & I2C_ISR_TXIS))
    {
      if (i2c->ISR & I2C_ISR_NACKF)
        return I2C_RCDATA_NACK;
      timeout--;
      if (!timeout)
        return I2C_RCTIMEOUT;
    }
    i2c->TXDR = *data++;
  }

  return I2C_RCOK;
}

int I2CMasterTransfer(I2C_TypeDef *i2c, unsigned int address,
                      unsigned char *commands, unsigned int commands_count,
                      unsigned char *write_data, unsigned int write_count,
                      unsigned char *read_data, unsigned int read_count, unsigned int timeout)
{
  unsigned int nwrite = write_count + commands_count;
  unsigned int nbytes = nwrite == 0 ? read_count : nwrite;
  unsigned int temp;
  int rc;

  if ((write_count != 0 && write_data == NULL) || (read_count != 0 && read_data == NULL) ||
      (commands_count != 0 && commands == NULL))
    return I2C_RCARGUMENTS_ERROR;

  i2c->ICR |= 0x3F38; // clear all interrupt flags

  rc = WaitOnFlagUntilTimeout(i2c, I2C_ISR_BUSY, 0, timeout);
  if (rc != I2C_RCOK)
    return rc;

  i2c->CR2 &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN);
  temp = address | (nbytes << 16);
  if (read_count == 0 || nwrite == 0)
    temp |= I2C_CR2_AUTOEND;
  if (nwrite == 0)
    temp |= I2C_CR2_RD_WRN; // read request
  i2c->CR2 |= temp;
  i2c->CR2 |= I2C_CR2_START;//START condition generation

  rc = i2c_send(i2c, commands, commands_count, timeout);
  if (rc != I2C_RCOK)
    return rc;
  rc = i2c_send(i2c, write_data, write_count, timeout);
  if (rc != I2C_RCOK)
    return rc;

  if (read_count != 0)
  {
    rc = WaitOnFlagUntilTimeout(i2c, I2C_ISR_TC, I2C_ISR_TC, timeout);
    if (rc != I2C_RCOK)
      return rc;

    if (nwrite != 0)
    {
      i2c->CR2 &= ~I2C_CR2_NBYTES;
      i2c->CR2 |= read_count << 16;
      i2c->CR2 |= I2C_CR2_START | I2C_CR2_RD_WRN | I2C_CR2_AUTOEND;//START condition generation
    }

    while (read_count--)
    {
      rc = WaitOnFlagUntilTimeout(i2c, I2C_ISR_RXNE, I2C_ISR_RXNE, timeout);
      if (rc != I2C_RCOK)
        return rc;
      *read_data++ = (unsigned char)i2c->RXDR;
    }
  }

  temp = i2c->ISR & 0x3F10; // error flags
  if (temp)
    return (int)temp;

  return WaitOnFlagUntilTimeout(i2c, I2C_ISR_STOPF, I2C_ISR_STOPF, timeout);
}
