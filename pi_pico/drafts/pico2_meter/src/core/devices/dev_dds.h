#ifndef _DEV_DDS_H
#define _DEV_DDS_H

#include <generic_dds.h>
#include <devices.h>

#define DDS_DEVICE_ID 1

#define DDS_AD9833 1
#define DDS_SI5351 2
#define LEVEL_METER_NONE   0
#define LEVEL_METER_AD8307 1
#define LEVEL_METER_AD8310 2
#define LEVEL_METER_AD8313 3
#define LEVEL_METER_AD8317 4
#define LEVEL_METER_AD8318 5

typedef struct {
  unsigned char deviceId;
  int (*command)(unsigned char deviceId, DeviceObject *o, unsigned char cmd, dds_cmd *data);
  DdsConfig cfg;
  void *device_config;
} dev_dds;

int dds_get_config(DdsConfig *cfg, DeviceObject *o); //should be defined in hal.c
int dds_command(unsigned char deviceId, DeviceObject *o, unsigned char cmd, dds_cmd *data); //should be defined in hal.c
void dds_initializer(DeviceObject *o);
int dds_message_processor(DeviceObject *o, unsigned char *buffer, int len);
int dds_save_config(DeviceObject *o, void *buffer);

#endif
