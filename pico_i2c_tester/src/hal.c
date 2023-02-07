#include "board.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <spi_3wire_soft.h>

void SystemInit(void)
{
  i2c_init(i2c_default, 100000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
  gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

  gpio_init(SPI3_DIO_PIN);
  gpio_init(SPI3_CS_PIN);
  gpio_init(SPI3_CLK_PIN);
  gpio_put(SPI3_CS_PIN, true);
  gpio_put(SPI3_CLK_PIN, true);
  gpio_put(SPI3_DIO_PIN, false);
  gpio_set_dir(SPI3_CLK_PIN, true);
  gpio_set_dir(SPI3_CS_PIN, true);
  SPI3_DIO_SET;
  gpio_pull_up(SPI3_DIO_PIN);
}

void spi3_delay(void)
{
  sleep_us(1);
}