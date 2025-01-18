#ifndef _DEVICES_H
#define _DEVICES_H

#include "board.h"

typedef struct {
  const char *name;
  int device_id;
  int public_id;
  void* (*initializer)(int idx, void **data);
  int (*timer_event)(int idx, int step, void *config, void *data, unsigned char *buffer);
  int (*message_processor)(int idx, void *config, void *data, unsigned char *buffer, int len);
} Device;

extern Device *device_list[MAX_DEVICES];
extern void *device_data[MAX_DEVICES];
extern void *device_config[MAX_DEVICES];

void BuildDeviceList(void);
void InitDeviceLists(void);

#endif
