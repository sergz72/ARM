#include "board.h"
#include <spi.h>

void SPI_Init(SPI_TypeDef *instance, SPI_InitStruct *init)
{
  //todo
}
void SPI_Enable(SPI_TypeDef *instance)
{
  instance->CR1 |= SPI_CR1_SPE;
}

void SPI_Disable(SPI_TypeDef *instance)
{
  instance->CR1 &= ~SPI_CR1_SPE;
}

void I2S_Init(SPI_TypeDef *instance, I2S_InitStruct *init)
{
  unsigned int cfgr = SPI_I2SCFGR_I2SMOD | init->mode | init->standard | init->data_size;
  if (init->long_frame_sync)
    cfgr |= SPI_I2SCFGR_PCMSYNC;
  if (init->clock_polarity)
    cfgr |= SPI_I2SCFGR_CKPOL;
  if (init->channel_32bit)
    cfgr |= SPI_I2SCFGR_CHLEN;
  instance->I2SCFGR = cfgr;
  cfgr = init->i2sdiv / 2;
  if (init->master_clock_output_enable)
    cfgr |= SPI_I2SPR_MCKOE;
  if (init->i2sdiv & 1)
    cfgr |= SPI_I2SPR_ODD;
  instance->I2SPR = cfgr;
}

void I2S_Enable(SPI_TypeDef *instance)
{
  instance->I2SCFGR |= SPI_I2SCFGR_I2SE;
}

void I2S_Disable(SPI_TypeDef *instance)
{
  instance->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
}

