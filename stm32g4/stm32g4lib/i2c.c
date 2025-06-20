#include "board.h"
#include <i2c.h>

void I2C_Master_Init(I2C_TypeDef *instance, unsigned int timings)
{
  instance->TIMINGR = timings;
  instance->CR2 = I2C_CR2_NACK;
}

void I2C_Enable(I2C_TypeDef *instance)
{
  instance->CR1 |= I2C_CR1_PE;
}

void I2C_Disable(I2C_TypeDef *instance)
{
  instance->CR1 &= ~I2C_CR1_PE;
}

int I2C_WaitFlag(I2C_TypeDef *instance, unsigned int flag, unsigned int timeout)
{
  while (!(instance->ISR & flag) && timeout)
    timeout--;
  return timeout;
}

int I2C_GenerateStop(I2C_TypeDef *instance, int rc, unsigned int timeout)
{
  instance->CR2 |= I2C_CR2_STOP;
  I2C_WaitFlag(instance, I2C_ISR_STOPF, timeout);
  return rc;
}

int I2C_Write(I2C_TypeDef *instance, unsigned short address, unsigned char *wdata, unsigned int wsize, unsigned int timeout)
{
  unsigned int t = timeout;
  while (instance->ISR & I2C_ISR_BUSY && t)
    t--;
  if (!t)
    return 1;

  instance->ICR = 0x3F38; // clear all error flags

  unsigned int temp = instance->CR2 & ~(I2C_CR2_NBYTES | I2C_CR2_SADD);
  instance->CR2 = temp | (wsize << 16) | I2C_CR2_AUTOEND | address;

  instance->CR2 |= I2C_CR2_START; // generate start

  if (!I2C_WaitFlag(instance, I2C_ISR_NACKF | I2C_ISR_TXIS, timeout))
    return 1;
  if (instance->ISR & I2C_ISR_NACKF)
    return 2;

  while (wsize--)
  {
    instance->TXDR = *wdata++;
    if (wsize)
    {
      if (!I2C_WaitFlag(instance, I2C_ISR_NACKF | I2C_ISR_TXIS, timeout))
        return 3;
      if (instance->ISR & I2C_ISR_NACKF)
        return 4;
    }
  }

  return I2C_WaitFlag(instance, I2C_ISR_STOPF, timeout) ? 0 : 3;
}
