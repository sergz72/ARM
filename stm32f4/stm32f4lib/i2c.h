#ifndef I2C_H
#define I2C_H

void I2C_Master_Init(I2C_TypeDef *instance, unsigned int bus_speed);
void I2C_Enable(I2C_TypeDef *instance);
void I2C_Disable(I2C_TypeDef *instance);
int I2C_WaitFlag(I2C_TypeDef *instance, unsigned int flag, unsigned int timeout);
int I2C_WaitFlag2(I2C_TypeDef *instance, unsigned int flag, unsigned int timeout);
int I2C_GenerateStop(I2C_TypeDef *instance, int rc, unsigned int timeout);
int I2C_Transfer(I2C_TypeDef *instance, unsigned short address, unsigned char *wdata, unsigned int wsize,
                 unsigned char *rdata, unsigned int rsize, unsigned int timeout);

#endif
