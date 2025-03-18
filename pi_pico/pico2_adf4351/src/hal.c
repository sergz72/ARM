#include "board.h"
#include "pico/time.h"
#include "hardware/spi.h"

#define SPI_CSN_SET gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1)
#define SPI_CSN_CLR gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0)

void adf4351_write(int channel, unsigned int data)
{
  unsigned char cdata[4];
  cdata[0] = (unsigned char)(data >> 24);
  cdata[1] = (unsigned char)((data >> 16) & 0xFF);
  cdata[2] = (unsigned char)((data >> 8) & 0xFF);
  cdata[3] = (unsigned char)(data & 0xFF);
  SPI_CSN_CLR;
  spi_write_blocking(spi_default, cdata, 4);
  SPI_CSN_SET;
}

void SystemInit(void)
{
  gpio_init(ADF4351_CE_PIN);
  ADF4351_CE_SET(0);
  gpio_set_dir(ADF4351_CE_PIN, true);

  spi_init(spi_default, 500);
  gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
  gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
  SPI_CSN_SET;
  gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
}
