#ifndef _DEV_INA226_H_
#define _DEV_INA226_H_

typedef struct {
  int current, voltage;
} DEV_INA226Data;

typedef struct {
  int r;
} DEV_INA226Config;

#define INA226_DEVICE_ID 0x80

void* ina226_initializer(int idx, void **data);
int ina226_timer_event(int idx, int step, void* config, void *data, unsigned char *buffer);

#endif

