#ifndef STM32F401_METER_DEVICE_CONFIG_H
#define STM32F401_METER_DEVICE_CONFIG_H

int read_config_from_eeprom(int idx, void *cfg, int size);
int write_config_to_eeprom(int idx, void *cfg, int size);

#endif //STM32F401_METER_DEVICE_CONFIG_H
