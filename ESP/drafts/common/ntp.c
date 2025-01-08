#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "ntp.h"

static const char *TAG = "ntp";

static void initialize_sntp(void)
{
  esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
  esp_sntp_setservername(0, "pool.ntp.org");
  esp_sntp_init();
}

void obtain_time(void)
{
  time_t now;
  struct tm timeinfo;
  char strftime_buf[64];

  initialize_sntp();

  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET)
  {
    ESP_LOGI(TAG, "Waiting for system time to be set...");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
  ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
}
