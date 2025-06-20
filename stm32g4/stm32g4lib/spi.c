#include "board.h"
#include <spi.h>

static unsigned int calculate_br(unsigned int baud_rate)
{
  unsigned int coef = BOARD_PCLK2 / baud_rate;
  if (coef <= 2)
    return 0;
  if (coef <= 4)
    return 1;
  if (coef <= 8)
    return 2;
  if (coef <= 16)
    return 3;
  if (coef <= 32)
    return 4;
  if (coef <= 64)
    return 5;
  if (coef <= 128)
    return 6;
  return 7;
}

void SPI_Init(SPI_TypeDef *instance, const SPI_InitStruct *init)
{
  unsigned int temp = init->clock_phase ? SPI_CR1_CPOL : 0;
  if (init->clock_polarity)
    temp |= SPI_CR1_CPOL;
  if (init->lsb_first)
    temp |= SPI_CR1_LSBFIRST;
  if (init->rxonly)
    temp |= SPI_CR1_RXONLY;
  if (init->master)
    temp |= SPI_CR1_MSTR;
  if (init->software_slave_management)
    temp |= SPI_CR1_SSI;
  if (init->internal_slave_select)
    temp |= SPI_CR1_SSM;
  unsigned int br = calculate_br(init->baud_rate);
  instance->CR1 = temp | (br << 3);
  temp = (init->data_size - 1) << 8;
  if (init->ss_output_enable)
    temp |= SPI_CR2_SSOE;
  if (init->ti_mode)
    temp |= SPI_CR2_FRF;
  if (init->tx_dma_enable)
    temp |= SPI_CR2_TXDMAEN;
  if (init->rx_dma_enable)
    temp |= SPI_CR2_RXDMAEN;
  if (init->error_interrupt_enable)
    temp |= SPI_CR2_ERRIE;
  if (init->rxne_interrupt_enable)
    temp |= SPI_CR2_RXNEIE;
  if (init->txe_interrupt_enable)
    temp |= SPI_CR2_TXEIE;
  instance->CR2 = temp;
}

void SPI_Enable(SPI_TypeDef *instance)
{
  instance->CR1 |= SPI_CR1_SPE;
}

void SPI_Disable(SPI_TypeDef *instance)
{
  instance->CR1 &= ~SPI_CR1_SPE;
}

int SPI_Send8(SPI_TypeDef *instance, unsigned char data, unsigned int timeout)
{
  while (!(instance->SR & SPI_SR_TXE) && timeout)
    timeout--;
  if (!timeout)
    return 1;
  *((unsigned char*)&instance->DR) = data;
  return 0;
}

int SPI_Send16(SPI_TypeDef *instance, unsigned short data, unsigned int timeout)
{
  while (!(instance->SR & SPI_SR_TXE) && timeout)
    timeout--;
  if (!timeout)
    return 1;
  instance->DR = data;
  return 0;
}

void I2S_Init(SPI_TypeDef *instance, const I2S_InitStruct *init)
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

