#ifndef _I2C_H
#define _I2C_H

void i2c_master_init(void);
int i2c_write(unsigned char address, const unsigned char* data, unsigned int l);
int i2c_read(unsigned char address, unsigned char* data, unsigned int l);
int i2c_transfer(unsigned char address, const unsigned char *wdata, unsigned int wlen, unsigned char *rdata, unsigned int rlen);

#endif
