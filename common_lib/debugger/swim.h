#ifndef _SWIM_H_
#define _SWIM_H_

#define SWIM_CSR_DM        0x20
#define SWIM_CSR_SAFE_MASK 0x80
#define SWIM_CSR           0x7F80

#define STM8_REG_A   0x7F00
#define STM8_REG_PCE 0x7F01
#define STM8_REG_PCH 0x7F02
#define STM8_REG_PCL 0x7F03
#define STM8_REG_XH  0x7F04
#define STM8_REG_XL  0x7F05
#define STM8_REG_YH  0x7F06
#define STM8_REG_YL  0x7F07
#define STM8_REG_SPH 0x7F08
#define STM8_REG_SPL 0x7F09
#define STM8_REG_CC  0x7F0A

#define SWIM_OK                  0
#define SWIM_ENTER_NO_SYNC_FRAME 1
#define SWIM_COMMAND_NO_ACK      2
#define SWIM_NBYTES_NO_ACK       3
#define SWIM_ADDR0_NO_ACK        4
#define SWIM_ADDR1_NO_ACK        5
#define SWIM_ADDR2_NO_ACK        6
#define SWIM_DATA_NO_ACK         7

#define SWIM_MAX_PACKET 1100

void swim_enter_critical_section(void);
void swim_leave_critical_section(void);
void swim_packet_handler(unsigned int clr);

void swim_init(unsigned int bclock);
unsigned int swim_enter(unsigned int *clock);
unsigned int swim_write(unsigned int address, unsigned char length, unsigned char *values);
unsigned int swim_read(unsigned int address, unsigned char length, unsigned char *values);
unsigned int swim_srst(void);
void swim_reset(void);
unsigned int swim_get_base_clock(void);

#endif
