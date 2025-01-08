//
// Created by sergi on 04.09.2022.
//

#ifndef TEMP_HUMI_PRES_SENSOR_WIFI_H
#define TEMP_HUMI_PRES_SENSOR_WIFI_H

#include "esp_wifi.h"

#define WIFI_NAME_LENGTH 32
#define WIFI_PASSWORD_LENGTH 64

extern wifi_config_t wifi_config;

int wifi_connect(void);
void wifi_init(void);

#endif //TEMP_HUMI_PRES_SENSOR_WIFI_H
