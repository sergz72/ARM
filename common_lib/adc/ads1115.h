#ifndef _ADS1115_H
#define _ADS1115_H

typedef struct {
  int os;
  int single_shot_mode;
  int comp_mode_window;
  int comp_active_high;
  int comp_latching;
  unsigned short mux;
  unsigned short pga;
  unsigned short dr;
  unsigned short comp_que;
} ADS1115Config;

#define ADS1115_MUX_AIN0_AIN1 0
#define ADS1115_MUX_AIN0_AIN3 (1 << 12)
#define ADS1115_MUX_AIN1_AIN3 (2 << 12)
#define ADS1115_MUX_AIN2_AIN3 (3 << 12)
#define ADS1115_MUX_AIN0_GND  (4 << 12)
#define ADS1115_MUX_AIN1_GND  (5 << 12)
#define ADS1115_MUX_AIN2_GND  (6 << 12)
#define ADS1115_MUX_AIN3_GND  (7 << 12)

#define ADS1115_PGA_6144 0
#define ADS1115_PGA_4096 (1 << 9)
#define ADS1115_PGA_2048 (2 << 9)
#define ADS1115_PGA_1024 (3 << 9)
#define ADS1115_PGA_512  (4 << 9)
#define ADS1115_PGA_256  (5 << 9)

#define ADS1115_DR_8  0
#define ADS1115_DR_16 (1 << 5)
#define ADS1115_DR_32 (2 << 5)
#define ADS1115_DR_64 (3 << 5)
#define ADS1115_DR_128 (4 << 5)
#define ADS1115_DR_250 (5 << 5)
#define ADS1115_DR_475 (6 << 5)
#define ADS1115_DR_860 (7 << 5)

#define ADS1115_COMP_QUE_ASSERT_AFTER_ONE  0
#define ADS1115_COMP_QUE_ASSERT_AFTER_TWO  1
#define ADS1115_COMP_QUE_ASSERT_AFTER_FOUR 2
#define ADS1115_COMP_QUE_DISABLE           3

int ads1115ReadRegister(int channel, unsigned char address, unsigned char reg, unsigned short *data);
int ads1115WriteRegister(int channel, unsigned char address, unsigned char reg, unsigned short data);

int ads1115SetConfig(int channel, unsigned char address, const ADS1115Config *cfg);
// bus voltage is in uV
int ads1115GetVoltage(int channel, unsigned char address, unsigned short pga, int *voltage);

#endif
