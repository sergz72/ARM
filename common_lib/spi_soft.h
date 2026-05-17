#ifndef _SPI_SOFT_H
#define _SPI_SOFT_H

unsigned char spi_byte(int channel, unsigned char data);
void spi_command(int channel, unsigned char cmd, unsigned char *data_in, unsigned char *data_out, int count, int set_cs);
void spi_transfer(int channel, unsigned char *data_in, unsigned char *data_out, int count, int set_cs);
void spi_channel_init(int channel, int cpha);
void spi_delay(int channel);

#endif
