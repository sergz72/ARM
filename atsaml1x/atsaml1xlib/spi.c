#include "board.h"
#include <spi.h>

void spi_master_init(void)
{
  //MISO - in
  PORT_REGS->GROUP[0].PORT_PINCFG[SPI_MASTER_MISO_PIN] = PORT_PINCFG_PMUXEN(1) | PORT_PINCFG_INEN(1) | PORT_PINCFG_PULLEN(1); // PMUXEN
  PORT_REGS->GROUP[0].PORT_PMUX[SPI_MASTER_MISO_PIN/2] |= SPI_MASTER_PMUX << (4 * (SPI_MASTER_MISO_PIN & 1));
  // SCK - out
  PORT_REGS->GROUP[0].PORT_PINCFG[SPI_MASTER_SCK_PIN] = PORT_PINCFG_PMUXEN(1);
  PORT_REGS->GROUP[0].PORT_PMUX[SPI_MASTER_SCK_PIN/2] |= SPI_MASTER_PMUX << (4 * (SPI_MASTER_SCK_PIN & 1));
  // MOSI - out
  PORT_REGS->GROUP[0].PORT_PINCFG[SPI_MASTER_MOSI_PIN] = PORT_PINCFG_PMUXEN(1);
  PORT_REGS->GROUP[0].PORT_PMUX[SPI_MASTER_MOSI_PIN/2] |= SPI_MASTER_PMUX << (4 * (SPI_MASTER_MOSI_PIN & 1));
  // NSS - out
  PORT_REGS->GROUP[0].PORT_OUTSET = 1 << SPI_MASTER_NSS_PIN;
  PORT_REGS->GROUP[0].PORT_DIRSET = 1 << SPI_MASTER_NSS_PIN;

  /* Selection of the Character Size and Receiver Disable */
  SPI_MASTER_REGS->SPIM.SERCOM_CTRLB = SERCOM_SPIM_CTRLB_CHSIZE_8_BIT | SERCOM_SPIM_CTRLB_RXEN(SPI_MASTER_RXEN) |
                                       SERCOM_SPIM_CTRLB_MSSEN(SPI_MASTER_MSSEN);

  /* Wait for synchronization */
  while((SPI_MASTER_REGS->SPIM.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }

  /* Selection of the Baud Value */
  SPI_MASTER_REGS->SPIM.SERCOM_BAUD = (uint8_t)SERCOM_SPIM_BAUD_BAUD(SPI_MASTER_BAUD_VALUE);

  /* Configure Data Out Pin, Master Mode,
   * Data In and Pin Out,Data Order and Standby mode if configured
   * and Selection of the Clock Phase and Polarity and Enable the SPI Module
   */
  SPI_MASTER_REGS->SPIM.SERCOM_CTRLA = SERCOM_SPIM_CTRLA_MODE_SPI_MASTER | SPI_MASTER_DOPO_PAD |
    SPI_MASTER_DIPO_PAD | SERCOM_SPIM_CTRLA_CPOL_IDLE_LOW | SERCOM_SPIM_CTRLA_CPHA_LEADING_EDGE |
      SERCOM_SPIM_CTRLA_DORD_MSB | SERCOM_SPIM_CTRLA_ENABLE(1) ;


  /* Wait for synchronization */
  while((SERCOM1_REGS->SPIM.SERCOM_SYNCBUSY) != 0U)
  {
    /* Do nothing */
  }
}

void spi_master_transfer(const unsigned char *wdata, unsigned char *rdata, unsigned int length)
{
  /* Flush out any unread data in SPI read buffer */
  while (SPI_MASTER_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_RXC_Msk)
  {
    uint32_t dummyData = SPI_MASTER_REGS->SPIM.SERCOM_DATA;
    (void)dummyData;
  }

  SPI_MASTER_REGS->SPIM.SERCOM_STATUS |= SERCOM_SPIM_STATUS_BUFOVF_Msk;

  SPI_MASTER_REGS->SPIM.SERCOM_INTFLAG |= SERCOM_SPIM_INTFLAG_ERROR_Msk;

  while (length--)
  {
    /* Make sure DRE is empty */
    while((SPI_MASTER_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk) != SERCOM_SPIM_INTFLAG_DRE_Msk)
    {
      /* Do nothing */
    }
    SPI_MASTER_REGS->SPIM.SERCOM_DATA = *wdata++;
    while (!(SPI_MASTER_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_RXC_Msk))
      ;
    *rdata++ = SPI_MASTER_REGS->SPIM.SERCOM_DATA;
  }
}
