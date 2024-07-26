#ifndef _I2C_H
#define _I2C_H

#define I2C_RCOK 0
#define I2C_RCTIMEOUT 1
#define I2C_RCARGUMENTS_ERROR 2
#define I2C_RCDATA_NACK 4

#define I2C_DEFAULT_TIMEOUT 0xFFFFF

int I2CMasterTransfer(I2C_TypeDef *i2c, unsigned int address,
                      unsigned char *commands, unsigned int commands_count,
                      unsigned char *write_data, unsigned int write_count,
                      unsigned char *read_data, unsigned int read_count, unsigned int timeout);

#endif
