#include "board.h"
#include <i2c.h>

static void i2c_timings_set(I2C_TypeDef *instance, unsigned int bus_speed)
{
  instance->CR2 = PCLK1_FREQ / 1000000;
  unsigned int ccr;
  if (bus_speed > 100000) // fast mode
  {
    unsigned int t = (PCLK1_FREQ / 3) / bus_speed;
    ccr = 0x8000 | t;
  }
  else // standard mode
  {
    unsigned int t = (PCLK1_FREQ / 2) / bus_speed;
    ccr = t;
  }
  if (ccr < 4)
    ccr = 4;
  instance->CCR = ccr;
}

void I2C_Master_Init(I2C_TypeDef *instance, unsigned int bus_speed)
{
  i2c_timings_set(instance, bus_speed);
  instance->TRISE = PCLK1_FREQ / 1000000 + 1;
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
  while (!(instance->SR1 & flag) && timeout)
    timeout--;
  return timeout;
}

int I2C_WaitFlagN2(I2C_TypeDef *instance, unsigned int flag, unsigned int timeout)
{
  while ((instance->SR2 & flag) && timeout)
    timeout--;
  return timeout;
}

int I2C_GenerateStop(I2C_TypeDef *instance, int rc, unsigned int timeout)
{
  instance->CR1 |= I2C_CR1_STOP;
  I2C_WaitFlagN2(instance, I2C_SR2_BUSY, timeout);
  return rc;
}

int I2C_Transfer(I2C_TypeDef *instance, unsigned short address, unsigned char *wdata, unsigned int wsize,
                 unsigned char *rdata, unsigned int rsize, unsigned int timeout)
{
  instance->SR1 = 0; // clear all error flags

  //instance->CR1 |= I2C_CR1_ACK;

  instance->CR1 |= I2C_CR1_START; // generate start
  if (!I2C_WaitFlag(instance, I2C_SR1_SB, timeout))
    return 1;
  if (wsize)
    instance->DR = address;
  else
    instance->DR = address | 1;
  if (!I2C_WaitFlag(instance, I2C_SR1_ADDR|I2C_SR1_AF, timeout))
    return I2C_GenerateStop(instance, 2, timeout);
  if (instance->SR1 & I2C_SR1_AF)
    return I2C_GenerateStop(instance, 3, timeout);
  volatile unsigned int dummy = instance->SR2; // clear ADDR flag
  (void)dummy;

  for (unsigned int i = 0; i < wsize; i++)
  {
    if (!I2C_WaitFlag(instance, I2C_SR1_TXE, timeout))
      return I2C_GenerateStop(instance, 4, timeout);
    instance->DR = *wdata++;
    if (!I2C_WaitFlag(instance, I2C_SR1_BTF|I2C_SR1_AF, timeout))
      return I2C_GenerateStop(instance, 5, timeout);
    if (instance->SR1 & I2C_SR1_AF)
      return I2C_GenerateStop(instance, 6, timeout);
  }

  if (wsize && rsize)
  {
    instance->CR1 |= I2C_CR1_ACK;
    instance->CR1 |= I2C_CR1_START; // generate start
    if (!I2C_WaitFlag(instance, I2C_SR1_SB, timeout))
      return I2C_GenerateStop(instance, 7, timeout);
    instance->DR = address | 1;
    if (!I2C_WaitFlag(instance, I2C_SR1_ADDR|I2C_SR1_AF, timeout))
      return I2C_GenerateStop(instance, 8, timeout);
    if (instance->SR1 & I2C_SR1_AF)
      return I2C_GenerateStop(instance, 9, timeout);
    volatile unsigned int dummy2 = instance->SR2; // clear ADDR flag
    (void)dummy2;
  }

  while (rsize--)
  {
    if (!rsize)
      instance->CR1 &= ~I2C_CR1_ACK;
    if (!I2C_WaitFlag(instance, I2C_SR1_RXNE, timeout))
      return I2C_GenerateStop(instance, 10, timeout);
    *rdata++ = instance->DR;
  }

  return I2C_GenerateStop(instance, 0, timeout);
}
