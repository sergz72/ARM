#ifndef _24C02_H
#define _24C02_H

#define _24C02_address(a) (0xA0 + (a << 1))
#define _24C02_page_size 8

unsigned int _24C02_write(unsigned int device_address, unsigned char word_address, unsigned char *data, unsigned int data_length);
unsigned int _24C02_read(unsigned int device_address, unsigned char word_address, unsigned char *buffer, unsigned int bytes_count);

#endif
