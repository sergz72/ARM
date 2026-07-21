#ifndef _I2C_H
#define _I2C_H

void i2c_master_init(void);
int i2c_write(unsigned char address, const unsigned char* data, unsigned int l);
int i2c_write2(unsigned char address, const unsigned char* data1, unsigned int l1, const unsigned char* data2, unsigned int l2);
int i2c_read(unsigned char address, unsigned char* data, unsigned int l);
int i2c_transfer(unsigned char address, const unsigned char *wdata, unsigned int wlen, unsigned char *rdata, unsigned int rlen);
int i2c_memory_write_page(unsigned char i2c_address, unsigned int memory_address,
                          unsigned int memory_address_length, const unsigned char *data, unsigned int length);
int i2c_memory_write_pages(unsigned char i2c_address, unsigned int memory_address,
                      unsigned int memory_address_length, unsigned int page_size,
                      const unsigned char *data, unsigned int length);
int i2c_memory_read(unsigned char i2c_address, unsigned int memory_address,
                      unsigned int memory_address_length, unsigned char *data, unsigned int length);

#endif
