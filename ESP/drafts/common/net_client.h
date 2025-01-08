#ifndef TEMP_HUMI_PRES_SENSOR_NET_CLIENT_H
#define TEMP_HUMI_PRES_SENSOR_NET_CLIENT_H

#include "crypto.h"

#define SERVER_ADDRESS_LENGTH 20
#define CONFIG_SERVER_PORT 55555

typedef struct __attribute__((__packed__)) {
  char address[SERVER_ADDRESS_LENGTH];
  unsigned short port;
  unsigned char aes_key[AES_KEY_LENGTH];
} server_parameters;

void send_env(void *data, int len);
void net_client_init(void);
void net_client_common_init(void);
void net_client_update_server_parameters(void);
void net_client_config_server_init(void);

extern server_parameters server_params;
extern struct sockaddr_in dest_addr;

#endif //TEMP_HUMI_PRES_SENSOR_NET_CLIENT_H
