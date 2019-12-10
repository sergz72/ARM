#ifndef _TH02_H
#define _TH02_H

#define TH02_ADDRESS 0x80

unsigned int th02Read(unsigned char register_no, unsigned char *pData, int size);
unsigned int th02Write(unsigned char register_no, unsigned char value);
unsigned int th02_check(void);
unsigned int th02_get_temperature(int *result);
unsigned int th02_get_humidity(unsigned int *result);

#endif
