#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "pins_config.h"

extern esp_err_t pmu_init();
extern esp_err_t pmu_i2c_init(void);

static const char *TAG = "main";

static unsigned int s_led_state;

static void configure_led(void)
{
  gpio_reset_pin(PIN_LED);
  /* Set the GPIO as a push/pull output */
  gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);

  s_led_state = 0;
}

static void blink_led(void)
{
  s_led_state = !s_led_state;
  gpio_set_level(PIN_LED, s_led_state);
}

void app_main(void)
{
  //vTaskDelay(5000 / portTICK_PERIOD_MS);

  int delay = 50;
  if (pmu_i2c_init() == ESP_OK)
  {
    ESP_LOGI(TAG, "PMU I2C initialized successfully");
    delay = 200;
    if (pmu_init() == ESP_OK)
      delay = 500;
  }


  configure_led();

  for (;;)
  {
    vTaskDelay(delay / portTICK_PERIOD_MS);
    blink_led();
  }
}
