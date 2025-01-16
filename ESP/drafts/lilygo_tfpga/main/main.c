#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "board.h"
#include "devices.h"
#include "driver/uart.h"

extern esp_err_t pmu_init();
extern esp_err_t pmu_i2c_init(void);

static const char *TAG = "main";

static unsigned char uart_buffer[UART_BUFFER_SIZE];

static void error(unsigned long ms)
{
  unsigned long v = ms / portTICK_PERIOD_MS;

  for (;;)
  {
    vTaskDelay(v);
    blink_led();
  }
}

void app_main(void)
{
  //vTaskDelay(5000 / portTICK_PERIOD_MS);

  configure_led();

#ifdef PMU_I2C_MASTER_NUM
  if (pmu_i2c_init() != ESP_OK)
    error(50);
  ESP_LOGI(TAG, "PMU I2C initialized successfully");
  if (pmu_init() != ESP_OK)
    error(200);
#endif

  configure_i2c();

  vTaskDelay(1000 / portTICK_PERIOD_MS); // time for peripheral devices to initialize

  BuildDeviceList();

  configure_uart();

  unsigned long start_time = xTaskGetTickCount();

  for (;;)
  {
    int len = uart_read_bytes(UART_PORT, uart_buffer, UART_BUFFER_SIZE, 10 / portTICK_PERIOD_MS);
    if (len > 0)
    {
      uart_write_bytes(UART_PORT, uart_buffer, len);
    }
    unsigned long new_time = xTaskGetTickCount();
    if (new_time - start_time >= 1000 / portTICK_PERIOD_MS)
    {
      blink_led();
      start_time = new_time;
    }
  }
}
