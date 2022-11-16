#ifndef AD9958_H_
#define AD9958_H_

#define AD9958_CSR_REGISTER 0
#define AD9958_FR1_REGISTER 1
#define AD9958_FR2_REGISTER 2
#define AD9958_CFR_REGISTER 3
#define AD9958_CFTW0_REGISTER 4
#define AD9958_CPOW0_REGISTER 5
#define AD9958_ACR_REGISTER 6
#define AD9958_LSRR_REGISTER 7
#define AD9958_RDW_REGISTER 8
#define AD9958_FDW_REGISTER 9
#define AD9958_CW1_REGISTER 10
#define AD9958_CW2_REGISTER 11
#define AD9958_CW3_REGISTER 12
#define AD9958_CW4_REGISTER 13
#define AD9958_CW5_REGISTER 14
#define AD9958_CW6_REGISTER 15
#define AD9958_CW7_REGISTER 16
#define AD9958_CW8_REGISTER 17
#define AD9958_CW9_REGISTER 18
#define AD9958_CW10_REGISTER 19
#define AD9958_CW11_REGISTER 20
#define AD9958_CW12_REGISTER 21
#define AD9958_CW13_REGISTER 22
#define AD9958_CW14_REGISTER 23
#define AD9958_CW15_REGISTER 24

void AD9958_Init(void);
unsigned char AD9958_Read8(unsigned char instruction);
unsigned short AD9958_Read16(unsigned char instruction);
unsigned int AD9958_Read24(unsigned char instruction);
unsigned int AD9958_Read32(unsigned char instruction);
void AD9958_Write8(unsigned char instruction, unsigned char data);
void AD9958_Write16(unsigned char instruction, unsigned short data);
void AD9958_Write24(unsigned char instruction, unsigned int data);
void AD9958_Write32(unsigned char instruction, unsigned int data);
void AD9958_Set(unsigned int channel, unsigned int ftw, unsigned short asf);

#endif /*AD9958_H_*/
