#ifndef LED_STRIP
#include "driver/gpio.h"
#endif
#include "led_strip.h"
#include "led.h"
#include "common.h"

void set_led_red(void)
{
  set_led(16, 0, 0);
}

void set_led_green(void)
{
  set_led(0, 16, 0);
}

void set_led_blue(void)
{
  set_led(0, 0, 16);
}

void set_led_yellow(void)
{
  set_led(8, 8, 0);
}

void set_led_purple(void)
{
  set_led(8, 0, 8);
}

void set_led_white(void)
{
  set_led(5, 5, 5);
}

#ifdef LED_STRIP
static led_strip_handle_t led_strip;

void set_led(uint32_t red, uint32_t green, uint32_t blue)
{
  /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
  led_strip_set_pixel(led_strip, 0, red, green, blue);
  /* Refresh the strip to send data */
  led_strip_refresh(led_strip);
}

void led_off(void)
{
  led_strip_clear(led_strip);
}

void configure_led(void)
{
  /* LED strip initialization with the GPIO and pixels number*/
  led_strip_config_t strip_config = {
      .strip_gpio_num = LED_GPIO,
      .max_leds = 1, // at least one LED on board
  };
  led_strip_rmt_config_t rmt_config = {
      .resolution_hz = 10 * 1000 * 1000, // 10MHz
  };
  ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
  /* Set all LED off to clear all pixels */
  led_strip_clear(led_strip);
}
#else
void set_led(uint32_t red, uint32_t green, uint32_t blue)
{
  gpio_set_level(LED_GPIO, red || green || blue);
}

void led_off(void)
{
  gpio_set_level(LED_GPIO, 0);
}

void configure_led(void)
{
  gpio_reset_pin(LED_GPIO);
  /* Set the GPIO as a push/pull output */
  gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
}
#endif
