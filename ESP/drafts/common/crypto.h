#ifndef TEMP_HUMI_PRES_SENSOR_CRYPTO_H
#define TEMP_HUMI_PRES_SENSOR_CRYPTO_H

#include <aes/esp_aes.h>

#define AES_KEY_LENGTH 16

int encrypt_env(void **data);
void crypto_init(void);

extern const unsigned char default_aes_key[];
extern esp_aes_context aes_ctx;

#endif //TEMP_HUMI_PRES_SENSOR_CRYPTO_H
