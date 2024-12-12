#include <hardware/spi.h>
#include <pico/time.h>
#include "board.h"

unsigned int spi_clock;

unsigned int spi_get_clock(int channel)
{
  return spi_clock;
}

void spi_set_clock(int channel, unsigned int value)
{
  spi_clock = spi_init(SPI_INST, value);
}

void spi_trfr(int channel, int nwrite, const unsigned char *wdata, int nread, unsigned char *rdata)
{
  gpio_put(SPI_CS_PIN, 0);

  spi_write_blocking(SPI_INST, wdata, nwrite);
  spi_read_blocking(SPI_INST, 0, rdata, nread);

  spi_finish(channel);
}

void spi_write(int channel, const unsigned char *data, int length)
{
  spi_write_blocking(SPI_INST, data, length);
}

void spi_read(int channel, unsigned char *data, int length)
{
  spi_read_blocking(SPI_INST, 0, data, length);
}

void spi_command(int channel, unsigned char cmd, unsigned char *data_in, unsigned char *data_out, int count, int set_cs)
{
  gpio_put(SPI_CS_PIN, 0);

  spi_write_blocking(SPI_INST, &cmd, 1);
  if (!data_in)
    spi_read_blocking(SPI_INST, 0, data_out, count);
  else if (!data_out)
    spi_write_blocking(SPI_INST, data_in, count);
  else
    spi_write_read_blocking(SPI_INST, data_in, data_out, count);

  if (set_cs)
    spi_finish(channel);
}

void spi_finish(int channel)
{
  gpio_put(SPI_CS_PIN, 1);
}

void _93CXX_delay(void)
{
  sleep_us(1);
}

static void LEDInit(void)
{
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void SystemInit(void)
{
  LEDInit();

  spi_clock = spi_init(SPI_INST, 1000000);

  gpio_set_function(SPI_SI_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SPI_CLK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SPI_SO_PIN, GPIO_FUNC_SPI);

  // Chip select is active-low, so we'll initialise it to a driven-high state
  gpio_init(SPI_CS_PIN);
  gpio_put(SPI_CS_PIN, 1);
  gpio_set_dir(SPI_CS_PIN, GPIO_OUT);

  gpio_init(_93CXX_DO_PIN);
  gpio_set_dir(_93CXX_DO_PIN, GPIO_IN);
  gpio_pull_up(_93CXX_DO_PIN);
  gpio_init(_93CXX_DI_PIN);
  gpio_set_dir(_93CXX_DI_PIN, GPIO_OUT);
  gpio_init(_93CXX_CS_PIN);
  gpio_set_dir(_93CXX_CS_PIN, GPIO_OUT);
  gpio_init(_93CXX_CLK_PIN);
  gpio_set_dir(_93CXX_CLK_PIN, GPIO_OUT);
}