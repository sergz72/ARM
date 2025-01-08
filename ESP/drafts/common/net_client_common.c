#include <net_client.h>
#include "lwip/sockets.h"
#include "nvs.h"
#include "crypto.h"
#include "wifi.h"
#include "esp_log.h"
#include "common.h"

static const char *TAG = "net_client";
static const char *OK = "Ok";

struct sockaddr_in dest_addr;
static struct sockaddr_in config_server_addr;
static char config_server_ip[SERVER_ADDRESS_LENGTH];
server_parameters server_params;

const unsigned char config_get_command = 0;

void net_client_config_server_init(void)
{
  esp_netif_ip_info_t ip_info;
  struct esp_netif_obj *netif;

  netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
  if (esp_netif_get_ip_info(netif, &ip_info) == ESP_OK)
  {
    ip_info.ip.addr = (ip_info.ip.addr & 0x00FFFFFF) | (CONFIG_SERVER_ADDRESS_SUFFIX << 24);
    memcpy(&config_server_addr.sin_addr.s_addr, &ip_info.ip, 4);
    config_server_addr.sin_family = AF_INET;
    config_server_addr.sin_port = htons(CONFIG_SERVER_PORT);
    inet_ntoa_r(ip_info.ip, config_server_ip, SERVER_ADDRESS_LENGTH);
    ESP_LOGI(TAG, "configuration server address %s", config_server_ip);
  }
  else
  {
    ESP_LOGI(TAG, "failed to get configuration server address");
    config_server_addr.sin_addr.s_addr = 0;
  }
}

void net_client_common_init(void)
{
  dest_addr.sin_addr.s_addr = inet_addr(server_params.address);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(server_params.port);
}

void net_client_update_server_parameters(void)
{
  server_parameters params;
  ssize_t rc;
  int sock;
  struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
  socklen_t socklen;
  struct timeval timeout;

  if (config_server_addr.sin_addr.s_addr)
  {
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0)
    {
      ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
      return;
    }

    // Set timeout
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

    ESP_LOGI(TAG, "Socket created, sending to %s:%d", config_server_ip, CONFIG_SERVER_PORT);
    rc = sendto(sock, &config_get_command, 1, 0, (struct sockaddr *) &config_server_addr, sizeof(config_server_addr));
    if (rc < 0)
      ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    else
    {
      ESP_LOGI(TAG, "Message sent");
      socklen = sizeof(source_addr);
      rc = recvfrom(sock, &params, sizeof(server_parameters), 0, (struct sockaddr *)&source_addr, &socklen);
      if (rc < 0)
        ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
      else if (rc == 0)
        ESP_LOGE(TAG, "Connection closed");
      else if (rc != sizeof(server_parameters))
        ESP_LOGE(TAG, "Wrong receive size");
      else
      {
        ESP_LOGI(TAG, "New config received");
        memcpy(&server_params, &params, sizeof(server_parameters));
        net_client_common_init();
        crypto_init();
        nvs_set_server_parameters();
        rc = sendto(sock, OK, 2, 0, (struct sockaddr *) &config_server_addr, sizeof(config_server_addr));
        if (rc < 0)
          ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        else
          ESP_LOGI(TAG, "Reply sent");
      }
    }
    shutdown(sock, 0);
    close(sock);
  }
}