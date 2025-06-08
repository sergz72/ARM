#ifndef I2C_FUNC_H
#define I2C_FUNC_H

#include <at32f403a_407_i2c.h>

int i2c_wait_until_not_busy(i2c_type *i2cx, unsigned int timeout);
int i2c_wait_flag_or_ackfail(i2c_type *i2cx, unsigned int flag, unsigned int timeout);
int i2c_master_write_addr(i2c_type *i2cx, unsigned short address, unsigned int timeout);
int i2c_master_transmit(i2c_type *i2cx, unsigned short address, unsigned char *pdata, unsigned int size, unsigned int timeout);

#endif
