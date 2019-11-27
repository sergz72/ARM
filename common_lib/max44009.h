#ifndef _MAX44009_H
#define _MAX44009_H

#define MAX44009_ADDR1 0x94
#define MAX44009_ADDR2 0x96

// MAX44009 register definitions
#define MAX44009_REG_INTS                      0x00 // Interrupt status
#define MAX44009_REG_INTE                      0x01 // Interrupt enable
#define MAX44009_REG_CFG                       0x02 // Configuration
#define MAX44009_REG_LUXH                      0x03 // Lux reading high byte
#define MAX44009_REG_LUXL                      0x04 // Lux reading low byte
#define MAX44009_REG_THU                       0x05 // Upper threshold high byte
#define MAX44009_REG_THL                       0x06 // Lower threshold high byte
#define MAX44009_REG_THT                       0x07 // Threshold timer

int max44009GetLuxH(unsigned short device_address, unsigned int *result);
int max44009GetLuxL(unsigned short device_address, unsigned int *result);

#endif
