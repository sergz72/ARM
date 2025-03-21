#include <board.h>
#include <spi.h>

#define SPI_FLAG_RXNE                   SPI_SR_RXNE   /* SPI status flag: Rx buffer not empty flag       */
#define SPI_FLAG_TXE                    SPI_SR_TXE    /* SPI status flag: Tx buffer empty flag           */
#define SPI_FLAG_BSY                    SPI_SR_BSY    /* SPI status flag: Busy flag                      */

#define _SPI_CLEAR_OVRFLAG(__INSTANCE__)   \
  do{                                      \
    __IO uint32_t tmpreg_ovr = 0x00U;      \
    tmpreg_ovr = (__INSTANCE__)->DR;       \
    tmpreg_ovr = (__INSTANCE__)->SR;       \
  } while(0U)

void SPI_Init(SPI_TypeDef *instance, SPI_InitTypeDef *init)
{
  /*----------------------- SPIx CR1 & CR2 Configuration ---------------------*/
  /* Configure : SPI Mode, Communication Mode, Data size, Clock polarity and phase, NSS management,
  Communication speed, First bit and CRC calculation state */
  WRITE_REG(instance->CR1, (init->Mode | init->Direction | init->DataSize |
                                  init->CLKPolarity | init->CLKPhase | (init->NSS & SPI_CR1_SSM) |
                                  init->BaudRatePrescaler | init->FirstBit  | init->CRCCalculation));

#if defined(SPI_CR2_FRF)
  if (init->TIMode == SPI_TIMODE_ENABLE)
    /* Configure : NSS management, TI Mode */
    WRITE_REG(instance->CR2, (((init->NSS >> 16U) & SPI_CR2_SSOE) | init->TIMode));
  else
#endif
    /* Configure : NSS management */
    WRITE_REG(instance->CR2, ((init->NSS >> 16U) & SPI_CR2_SSOE));

  /*---------------------------- SPIx CRCPOLY Configuration ------------------*/
  /* Configure : CRC Polynomial */
  if (init->CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    WRITE_REG(instance->CRCPR, init->CRCPolynomial);
  }

#if defined(SPI_I2SCFGR_I2SMOD)
  /* Activate the SPI mode (Make sure that I2SMOD bit in I2SCFGR register is reset) */
  CLEAR_BIT(instance->I2SCFGR, SPI_I2SCFGR_I2SMOD);
#endif /* SPI_I2SCFGR_I2SMOD */

  instance->CR1 |= SPI_CR1_SPE;
}

unsigned int SPI_TransmitReceive(SPI_TypeDef *instance, unsigned char *txData, unsigned char *rxData, unsigned int size, unsigned int timeout)
{
  unsigned int t;

  while (size--)
  {
    t = timeout;
    /* Check TXE flag */
    while (!(instance->SR & SPI_FLAG_TXE) && --t)
      ;
    if (!t)
      return 0;
    instance->DR = *txData++;
    t = timeout;
    /* Wait until RXNE flag is set */
    while (!(instance->SR & SPI_FLAG_RXNE) && --t)
      ;
    if (!t)
      return 0;
    *rxData++ = instance->DR;
  }

  /* Control the BSY flag */
  while ((instance->SR & SPI_FLAG_BSY) && --timeout)
    ;

  _SPI_CLEAR_OVRFLAG(instance);

  return timeout;
}
