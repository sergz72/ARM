#ifndef _DEVICES_H
#define _DEVICES_H

#include "board.h"

typedef struct {
  const char *name;
  int device_id;
  void* (*initializer)(int idx);
  void* (*data_collector)(int idx, int step, void *config, void *data);
} Device;

extern int device_list[MAX_DEVICES];
extern void *device_data[MAX_DEVICES];
extern void *device_config[MAX_DEVICES];

void BuildDeviceList(void);
void BuildDeviceData(int step);

#endif
