#ifndef _DEV_SI5351_H
#define _DEV_SI5351_H

#define SI5351_DEVICE_ID 0xC0

void* si5351_initializer(int idx, void **data);
int si5351_message_processor(int idx, void *config, void *data, unsigned char *buffer, int len);
int si5351_save_config(void *buffer);

#endif
