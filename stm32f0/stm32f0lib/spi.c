#include <spi.h>
#include <nvic.h>

static SPI_InitTypeDef *config[2];

void SPI_Init(SPI_TypeDef* spi, const SPI_InitTypeDef* init)
{
  unsigned int cr1 = init->BaudRateDivider << 3, cr2;

  if (spi == SPI1)
    config[0] = (SPI_InitTypeDef*)init;
  else
    config[1] = (SPI_InitTypeDef*)init;

  if (init->BidiMode)
    cr1 |= SPI_CR1_BIDIMODE;
  if (init->CRCEnable)
    cr1 |= SPI_CR1_CRCEN;
  if (init->CRC16Bit)
    cr1 |= SPI_CR1_CRCL;
  if (init->RXOnly)
    cr1 |= SPI_CR1_RXONLY;
  if (init->SoftwareSlaveManagement)
    cr1 |= SPI_CR1_SSM;
  if (init->LSBFirst)
    cr1 |= SPI_CR1_LSBFIRST;
  if (init->MasterMode)
    cr1 |= SPI_CR1_MSTR | SPI_CR1_SSI;
  if (init->CPOLHigh)
    cr1 |= SPI_CR1_CPOL;
  if (init->CPHA2Edge)
    cr1 |= SPI_CR1_CPHA;
  spi->CR1 = cr1;

  cr2 = (init->DataSize - 1) << 8;

  if (init->FIFOThreshold4)
    cr2 |= SPI_CR2_FRXTH;
  if (init->TIMode)
    cr2 |= SPI_CR2_FRF;
  if (init->TXEHandler)
    cr2 |= SPI_CR2_TXEIE;
  if (init->RXNEHandler)
    cr2 |= SPI_CR2_RXNEIE;
  if (init->ErrorInterruptEnable)
    cr2 |= SPI_CR2_ERRIE;
  if (init->NSSPulse)
    cr2 |= SPI_CR2_NSSP;
  if (init->SSOutputEnable)
    cr2 |= SPI_CR2_SSOE;
  if (init->TXDmaEnable)
    cr2 |= SPI_CR2_TXDMAEN;
  if (init->RXDmaEnable)
    cr2 |= SPI_CR2_RXDMAEN;
  spi->CR2 = cr2;

#ifdef SPI1_IRQ_ENABLE
  NVIC_Init(SPI1_IRQn, 3, 1, ENABLE);
#endif
#ifdef SPI2_IRQ_ENABLE
  NVIC_Init(SPI2_IRQn, 3, 1, ENABLE);
#endif

  cr1 |= SPI_CR1_SPE;
  spi->CR1 = cr1;
}

void SPI_SetCpolCpha(SPI_TypeDef* spi, int cpol, int cpha)
{
  unsigned int cr = spi->CR1 & 0xFFFC;
  if (cpol)
    cr |= SPI_CR1_CPOL;
  if (cpha)
    cr |= SPI_CR1_CPHA;
  spi->CR1 = cr;
}

unsigned short SPI_Send(SPI_TypeDef* spi, unsigned short data)
{
  while (!(spi->SR & SPI_SR_TXE))
    ;
  spi->DR = data;
  while (!(spi->SR & SPI_SR_RXNE))
    ;
  return spi->DR;
}

void SPI_BusyWait(SPI_TypeDef* spi)
{
  while (spi->SR & SPI_SR_BSY)
    ;
}

static void SPI_IRQHandler(SPI_TypeDef* spi, int configId)
{
  unsigned int flags = spi->SR;
  SPI_InitTypeDef *c = config[configId];
}

#ifdef SPI1_IRQ_ENABLE
void SPI1_IRQHandler(void)
{
  SPI_IRQHandler(SPI1, 0);
}
#endif

#ifdef SPI2_IRQ_ENABLE
void SPI2_IRQHandler(void)
{
  SPI_IRQHandler(SPI2, 1);
}
#endif
