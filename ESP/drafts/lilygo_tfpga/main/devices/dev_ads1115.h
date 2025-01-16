#ifndef _DEV_ADS1115_H
#define _DEV_ADS1115_H

#define ADS1115_DEVICE_ID 0x90

void* ads1115_initializer(int idx);
void *ads1115_data_collector(int idx, int step, void* config, void *data);

#endif
