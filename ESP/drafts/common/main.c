#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led.h"
#include "nvs.h"
#include "wifi.h"
#include "env.h"
#include "crypto.h"
#include "net_client.h"
#include "ntp.h"
#include "common.h"

void ble_init(void);

//static const char *TAG = "main";

static void initialise_button(void)
{
  gpio_config_t io_conf = {0};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.pin_bit_mask = BIT64(BUTTON_GPIO);
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pull_up_en = 1;
  io_conf.pull_down_en = 0;
  gpio_config(&io_conf);
}

static void button_wait(void)
{
  int timer = 5;
  // wait 4 seconds for key press
  while (--timer)
  {
    if (!gpio_get_level(BUTTON_GPIO))
    {
      vTaskDelay(50 / portTICK_PERIOD_MS);
      if (!gpio_get_level(BUTTON_GPIO))
      {
        break;
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  if (timer) // key was pressed
  {
    set_led_yellow();

    // wait for key release
    while (true)
    {
      if (gpio_get_level(BUTTON_GPIO))
      {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        if (gpio_get_level(BUTTON_GPIO))
        {
          break;
        }
      }
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    // wait for key press
    while (true)
    {
      if (!gpio_get_level(BUTTON_GPIO))
      {
        vTaskDelay(50 / portTICK_PERIOD_MS);
        if (!gpio_get_level(BUTTON_GPIO))
        {
          break;
        }
      }
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  }
}

void app_main(void)
{
  int l;
  void *data;

  configure_led();
  initialise_button();

  set_led_white();

  button_wait();

  init_env();

  nvs_init();
  wifi_init();
  if (wifi_connect())
  {
    set_led_blue();
    ble_init();
  }
  else
  {
    set_led_yellow();
    obtain_time();
    led_off();
    crypto_init();
    net_client_init();
    net_client_config_server_init();
    post_init_env();
    for (;;)
    {
      net_client_update_server_parameters();
      if (!get_env())
      {
        l = encrypt_env(&data);
        if (l > 0)
          send_env(data, l);
      }
      vTaskDelay(SEND_INTERVAL / portTICK_PERIOD_MS);
    }
  }
}
