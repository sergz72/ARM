#ifndef _24C01_16_H
#define _24C01_16_H

#define _24C01_16_address(a) (0xA0 + (a << 1))
#define _24C02_page_size 8
#define _24C04_page_size 16
#define _24C256_page_size 64

int _24C01_16_write(int channel, unsigned int device_address, unsigned char word_address, unsigned char *data,
                    unsigned int data_length, int timeout);
int _24C01_16_read(int channel, unsigned int device_address, unsigned char word_address, unsigned char *buffer,
                   unsigned int bytes_count, int timeout);

#endif
