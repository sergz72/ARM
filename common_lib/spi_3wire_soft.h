#ifndef _SPI_3WIRE_SOFT_H
#define _SPI_3WIRE_SOFT_H

void spi3_byte_out(unsigned char data, int count);
unsigned char spi3_byte_in(int count);
void spi3_command(unsigned char cmd, unsigned char *data_out, int count_out, unsigned char *data_in, int count_in,
                  int set_cs, int data_in_delay);
void spi3_delay(void);

#endif
