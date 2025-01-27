#ifndef _DEV_DDS_H
#define _DEV_DDS_H

#include <generic_dds.h>

#define DDS_AD9833 1
#define DDS_SI5351 2

typedef struct {
  unsigned char deviceId;
  int (*command)(unsigned char deviceId, unsigned char cmd, dds_cmd *data, int idx);
  dds_config cfg;
} dev_dds;

int dds_get_config(dds_config *cfg, unsigned char deviceId, int idx);
int dds_command(unsigned char deviceId, unsigned char cmd, dds_cmd *data, int idx);
void* dds_initializer(unsigned char deviceId, int idx);
int dds_message_processor(int idx, void *config, void *data, unsigned char *buffer, int len);
int dds_save_config(void *buffer);

#endif
