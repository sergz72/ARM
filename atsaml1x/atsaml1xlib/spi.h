#ifndef _SPI_H
#define _SPI_H

void spi_master_init(void);
void spi_master_transfer(const unsigned char *wdata, unsigned char *rdata, unsigned int length);

#endif
