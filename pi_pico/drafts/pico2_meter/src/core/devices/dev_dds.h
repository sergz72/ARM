#ifndef _DEV_DDS_H
#define _DEV_DDS_H

#include <generic_dds.h>
#include <devices.h>

#define DDS_AD9833 1
#define DDS_SI5351 2
#define LEVEL_METER_NONE   0
#define LEVEL_METER_AD8307 1

typedef struct {
  unsigned char deviceId;
  int (*command)(unsigned char deviceId, unsigned char cmd, dds_cmd *data, int idx, void *config);
  dds_config cfg;
  void *device_config;
} dev_dds;

int dds_get_config(dds_config *cfg, unsigned char deviceId, int idx); //should be defined in hal.c
int dds_command(unsigned char deviceId, unsigned char cmd, dds_cmd *data, int idx, void *config); //should be defined in hal.c
void dds_initializer(unsigned char deviceId, DeviceObject *o);
int dds_message_processor(DeviceObject *o, unsigned char *buffer, int len);

#endif
