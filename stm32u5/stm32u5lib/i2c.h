#ifndef I2C_H
#define I2C_H

void I2C_Master_Init(I2C_TypeDef *instance, unsigned int timings);
void I2C_Enable(I2C_TypeDef *instance);
void I2C_Disable(I2C_TypeDef *instance);
unsigned int I2C_WaitFlag(I2C_TypeDef *instance, unsigned int flag, unsigned int timeout);
int I2C_GenerateStop(I2C_TypeDef *instance, int rc, unsigned int timeout);
int I2C_Write(I2C_TypeDef *instance, unsigned short address, const unsigned char *wdata, unsigned int wsize,
               unsigned int timeout, bool generate_stop);
int I2C_Read(I2C_TypeDef *instance, unsigned char *rdata, unsigned int rsize, unsigned int timeout);

#endif
