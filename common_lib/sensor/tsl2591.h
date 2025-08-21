#ifndef _TSL2591_H
#define _TSL2591_H

#define TSL2591_I2C_ADDRESS 0x29

enum tsl2591_persist {EveryCycle, AnyOutOfRange, _2OutOfRange, _3OutOfRange, _5OutOfRange, _10OutOfRange,
                      _15OutOfRange, _20OutOfRange, _25OutOfRange, _30OutOfRange, _35OutOfRange, _40OutOfRange,
                      _45OutOfRange, _50OutOfRange, _55OutOfRange, _60OutOfRange};

typedef struct
{
  unsigned short low;
  unsigned short high;
} tsl2591_thresholds;

typedef struct
{
  int no_persist_interrupt_enable;
  int sleep_after_interrupt;
  int als_interrupt_enable;
  unsigned short integration_time_ms;
  tsl2591_thresholds als_thresholds;
  tsl2591_thresholds no_persist_als_thresholds;
  enum tsl2591_persist persistence_filter;
} tsl2591_config;

typedef struct
{
  float lux;
  unsigned int gain;
  unsigned int tries;
} tsl2591_result;

int tsl2591_read8(unsigned char reg, unsigned char *data); // should be defined in hal.c
int tsl2591_read16(unsigned char reg, unsigned short *data); // should be defined in hal.c
int tsl2591_write(unsigned char reg, unsigned char data); // should be defined in hal.c
int tsl2591_command(unsigned char command); // should be defined in hal.c

int tsl2591_init(const tsl2591_config *config);
int tsl2591_measure(tsl2591_result *result);
int tsl2591_clear_interrupts(void);
int tsl2591_get_status(unsigned char *status);
int tsl2591_set_no_persist_als_thresholds(const tsl2591_thresholds *thresholds);
int tsl2591_set_als_thresholds(const tsl2591_thresholds *thresholds);
int tsl2591_set_persistence_filter(unsigned char filter);
int tsl2591_enable(unsigned char enable);

#endif
