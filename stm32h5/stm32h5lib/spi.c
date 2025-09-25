#include "board.h"
#include <spi.h>

static unsigned int calculate_br(unsigned int baud_rate)
{
  unsigned int coef = SPI_CLOCK / baud_rate;
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
  unsigned int temp = init->software_slave_management ? SPI_CR1_SSI : 0;
  instance->CR1 = temp;
  temp = init->tx_dma_enable ? SPI_CFG1_TXDMAEN : 0;
  if (init->rx_dma_enable)
    temp |= SPI_CFG1_RXDMAEN;
  temp |= (init->data_size - 1) | (init->fifo_threshold << 5);
  unsigned int br = calculate_br(init->baud_rate);
  instance->CFG1 = temp | (br << 28);
  temp = init->alternate_function_gpio_control ? SPI_CFG2_AFCNTR : 0;
  if (init->ss_output_enable)
    temp |= SPI_CFG2_SSOE;
  if (init->internal_slave_select)
    temp |= SPI_CFG2_SSM;
  if (init->clock_polarity)
    temp |= SPI_CFG2_CPOL;
  if (init->clock_phase)
    temp |= SPI_CFG2_CPHA;
  if (init->lsb_first)
    temp |= SPI_CFG2_LSBFRST;
  if (init->master)
    temp |= SPI_CFG2_MASTER;
  temp |= init->comm_mode;
  if (init->io_swap)
    temp |= SPI_CFG2_IOSWP;
  if (init->ti_mode)
    temp |= SPI_CFG2_SP_0;
  instance->CFG2 = temp;

  temp = init->txtf_interrupt_enable ? SPI_IER_TXTFIE : 0;
  if (init->eot_interrupt_enable)
    temp |= SPI_IER_EOTIE;
  if (init->dxp_interrupt_enable)
    temp |= SPI_IER_DXPIE;
  if (init->txp_interrupt_enable)
    temp |= SPI_IER_TXPIE;
  if (init->rxp_interrupt_enable)
    temp |= SPI_IER_RXPIE;
  instance->IER = temp;
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
  while (!(instance->SR & SPI_SR_TXP) && timeout)
    timeout--;
  if (!timeout)
    return 1;
  *((unsigned char*)&instance->TXDR) = data;
  return 0;
}

int SPI_WaitSend(SPI_TypeDef *instance, unsigned int timeout)
{
  while (!(instance->SR & SPI_SR_TXC) && timeout)
    timeout--;
  return timeout ? 0 : 1;
}

int SPI_Send16(SPI_TypeDef *instance, unsigned short data, unsigned int timeout)
{
  while (!(instance->SR & SPI_SR_TXP) && timeout)
    timeout--;
  if (!timeout)
    return 1;
  instance->TXDR = data;
  return 0;
}

void I2S_Init(SPI_TypeDef *instance, const I2S_InitStruct *init)
{
  unsigned int cfgr = SPI_I2SCFGR_I2SMOD | init->mode | init->standard | init->data_size | init->configuration;
  if (init->long_frame_sync)
    cfgr |= SPI_I2SCFGR_PCMSYNC;
  if (init->clock_polarity)
    cfgr |= SPI_I2SCFGR_CKPOL;
  if (init->channel_32bit)
    cfgr |= SPI_I2SCFGR_CHLEN;
  if (init->master_clock_output_enable)
    cfgr |= SPI_I2SCFGR_MCKOE;
  cfgr |= (init->i2sdiv / 2) << 16;
  if (init->i2sdiv & 1)
    cfgr |= SPI_I2SCFGR_ODD;
  instance->I2SCFGR = cfgr;
}

void I2S_Enable(SPI_TypeDef *instance)
{
  instance->I2SCFGR |= SPI_I2SCFGR_I2SMOD;
}

void I2S_Disable(SPI_TypeDef *instance)
{
  instance->I2SCFGR &= ~SPI_I2SCFGR_I2SMOD;
}

