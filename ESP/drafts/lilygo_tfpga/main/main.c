#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "board.h"
#include "led_strip.h"
#include "devices.h"

extern esp_err_t pmu_init();
extern esp_err_t pmu_i2c_init(void);

static const char *TAG = "main";

static unsigned char uart_buffer[UART_BUFFER_SIZE];

static unsigned int s_led_state;

#ifdef LED_STRIP
static led_strip_handle_t led_strip;

static void configure_led(void)
{
  /* LED strip initialization with the GPIO and pixels number*/
  led_strip_config_t strip_config = {
      .strip_gpio_num = PIN_LED,
      .max_leds = 1, // at least one LED on board
  };
  led_strip_rmt_config_t rmt_config = {
      .resolution_hz = 10 * 1000 * 1000, // 10MHz
  };
  ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
  /* Set all LED off to clear all pixels */
  led_strip_clear(led_strip);

  s_led_state = 0;
}

static void set_led(uint32_t red, uint32_t green, uint32_t blue)
{
  /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
  led_strip_set_pixel(led_strip, 0, red, green, blue);
  /* Refresh the strip to send data */
  led_strip_refresh(led_strip);
}

static void blink_led(void)
{
  s_led_state = s_led_state ? 0 : 8;
  set_led(0, s_led_state, 0);
}
#else
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
#endif

static void error(unsigned long ms)
{
  unsigned long v = ms / portTICK_PERIOD_MS;

  for (;;)
  {
    vTaskDelay(v);
    blink_led();
  }
}

static void configure_uart(void)
{
  uart_config_t uart_config = {
      .baud_rate = UART_BAUD_RATE,
      .data_bits = UART_DATA_8_BITS,
      .parity    = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };

  uart_driver_install(UART_PORT, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 0, NULL, ESP_INTR_FLAG_IRAM);
  uart_param_config(UART_PORT, &uart_config);
  uart_set_pin(UART_PORT, PIN_UART_TXD, PIN_UART_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
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
