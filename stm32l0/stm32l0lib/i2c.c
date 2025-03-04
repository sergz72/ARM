#include <board.h>
#include <i2c.h>

unsigned int calculateTiming(unsigned int clockSpeed)
{
  unsigned int timing = PCLK1_FREQ / clockSpeed;
  unsigned int sclh, scll, presc;
  if (PCLK1_FREQ % clockSpeed)
    timing++;
  presc = timing / 512;
  timing >>= presc;
  sclh = scll = timing / 2;
  if (timing & 1)
    scll++;
  return scll | (sclh << 8) | (presc << 28);
}

void I2C_Init(I2C_TypeDef *instance, I2C_InitTypeDef *init)
{
  unsigned int cr1 = init->GeneralCallMode ? I2C_CR1_GCEN : 0;
  if (!init->AnalogNoiseFilter)
    cr1 |= I2C_CR1_ANFOFF;
  cr1 |= (init->DigitalNoiseFilter & 0xF) << 8;
  instance->CR1 = cr1;
  instance->TIMINGR = calculateTiming(init->ClockSpeed);
}

void I2C_Enable(I2C_TypeDef *instance)
{
  instance->CR1 |= I2C_CR1_PE;
}

void I2C_Disable(I2C_TypeDef *instance)
{
  instance->CR1 &= ~I2C_CR1_PE;
}

void I2C_TransferHandling(I2C_TypeDef* instance, unsigned short Address, unsigned int Number_Bytes, unsigned int ReloadEndMode, unsigned int StartStopMode)
{
  instance->CR2 = (Address & I2C_CR2_SADD) | ((Number_Bytes << 16 ) & I2C_CR2_NBYTES) | ReloadEndMode | StartStopMode;
}

unsigned int I2C_Test(I2C_TypeDef *instance, unsigned short DevAddress, unsigned int Timeout)
{
  return 0;
}