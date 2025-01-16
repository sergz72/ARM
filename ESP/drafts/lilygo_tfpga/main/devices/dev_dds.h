#ifndef _DEV_DDS_H
#define _DEV_DDS_H

#include <generic_dds.h>

typedef struct {
  unsigned int   frequency;
  unsigned short output_voltage;
  unsigned short output_divider_value;
  unsigned char  mode;
  unsigned char  enabled;
} dds_channel;

typedef struct {
  unsigned char deviceId;
  int current_channel;
  unsigned int default_frequency;
  int (*command)(unsigned char deviceId, unsigned char cmd, dds_cmd *data, int idx);
  dds_config cfg;
  dds_channel *channel_data;
} dev_dds;

int dds_get_config(dds_config *cfg, unsigned char deviceId, int idx);
int dds_command(unsigned char deviceId, unsigned char cmd, dds_cmd *data, int idx);
void dds_init_channel_data(dev_dds *ddds);
void* dds_initializer(unsigned char deviceId, unsigned int default_frequency, int idx);

#endif
