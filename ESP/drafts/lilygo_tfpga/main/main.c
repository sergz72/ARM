#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void app_main(void)
{
  ESP_ERROR_CHECK(pmu_i2c_init());
  ESP_LOGI(TAG, "PMU I2C initialized successfully");

  ESP_ERROR_CHECK(pmu_init());

  for (;;)
  {
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
