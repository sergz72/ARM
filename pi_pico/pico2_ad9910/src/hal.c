#include "board.h"
#include "pico/time.h"
#include "hardware/spi.h"

#define SPI_CSN_SET gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1)
#define SPI_CSN_CLR gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0)

void delay_us(unsigned int us)
{
  sleep_us(us);
}

void delay_ms(unsigned int ms)
{
  sleep_ms(ms);
}

void ad9910_write(int channel, unsigned char command, const unsigned char *data, unsigned int length)
{
  SPI_CSN_CLR;
  spi_write_blocking(spi_default, &command, 1);
  spi_write_blocking(spi_default, data, length);
  SPI_CSN_SET;
}

void SystemInit(void)
{
  gpio_init(AD9910_RESET_PIN);
  gpio_init(AD9910_EXT_PWR_DOWN_PIN);
  gpio_init(AD9910_PLL_LOCK_PIN);
  gpio_init(AD9910_IO_UPDATE_PIN);
  AD9910_RESET_SET(0);
  AD9910_EXT_PWR_DOWN_SET(0);
  AD9910_IO_UPDATE_CLR(0);
  gpio_set_dir(AD9910_RESET_PIN, true);
  gpio_set_dir(AD9910_EXT_PWR_DOWN_PIN, true);
  gpio_set_dir(AD9910_PLL_LOCK_PIN, false);
  gpio_pull_down(AD9910_PLL_LOCK_PIN);
  gpio_set_dir(AD9910_IO_UPDATE_PIN, true);

  spi_init(spi_default, 500);
  gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
  gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
  SPI_CSN_SET;
  gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
}
