#ifndef _SHT3X_H
#define _SHT3X_H

unsigned int sht3xRead(unsigned char address, unsigned short command, unsigned char *pData, int size);
unsigned int sht3xReadData(unsigned char address, unsigned char *pData, int size);
unsigned int sht3xCommand(unsigned char address, unsigned short command);
unsigned int sht3x_get_data(short *temperature, unsigned char *humidity);
unsigned int sht3x_read_status(unsigned short *status);

#endif
