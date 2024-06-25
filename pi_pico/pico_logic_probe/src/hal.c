#include "board.h"
#include <pico/time.h>
#include "hardware/spi.h"
#include "hardware/pwm.h"

unsigned int slice_num;

void ST7735_WriteBytes(unsigned char *data, unsigned int size)
{
  spi_write_blocking(ST7735_SPI_PORT, data, size);
}

void ST7735_SET_PWM(unsigned int duty)
{
  pwm_set_chan_level(slice_num, PWM_CHAN_B, duty);
}

void delayms(unsigned int ms)
{
  sleep_ms(ms);
}

static void InputInit(unsigned int pin)
{
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_IN);
  gpio_pull_up(pin);
}

void InputsCommand(void (*func)(unsigned int))
{
  func(DATA0);
  func(DATA1);
  func(DATA2);
  func(DATA3);
  func(DATA4);
  func(DATA5);
  func(DATA6);
  func(DATA7);
  func(DATA8);
  func(DATA9);
  func(DATA10);
  func(DATA11);
  func(DATA12);
}

static void LCDInit(void)
{
  spi_init(ST7735_SPI_PORT, 10000 * 1000);
  gpio_set_function(ST7735_CLK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(ST7735_MOSI_PIN, GPIO_FUNC_SPI);

  gpio_init(ST7735_RST_PIN);
  gpio_set_dir(ST7735_RST_PIN, GPIO_OUT);
  gpio_init(ST7735_DC_PIN);
  gpio_set_dir(ST7735_DC_PIN, GPIO_OUT);
  gpio_init(ST7735_CS_PIN);
  gpio_set_dir(ST7735_CS_PIN, GPIO_OUT);
  gpio_init(ST7735_BL_PIN);
  gpio_set_dir(ST7735_BL_PIN, GPIO_OUT);

  gpio_put(ST7735_CS_PIN, 1);
  gpio_put(ST7735_DC_PIN, 0);
  gpio_put(ST7735_BL_PIN, 1);

  // PWM Config
  gpio_set_function(ST7735_BL_PIN, GPIO_FUNC_PWM);
  slice_num = pwm_gpio_to_slice_num(ST7735_BL_PIN);
  pwm_set_wrap(slice_num, 100);
  pwm_set_chan_level(slice_num, PWM_CHAN_B, 1);
  pwm_set_clkdiv(slice_num,50);
  pwm_set_enabled(slice_num, true);
}

void LEDInit(void)
{
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void SystemInit(void)
{
  LCDInit();
  InputsCommand(InputInit);
  LEDInit();
}
