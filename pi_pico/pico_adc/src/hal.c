#include "board.h"
#include <hardware/spi.h>
#include <pico/time.h>
#include <string.h>
#include <ads1259.h>
#include <ad7124.h>
#include <ad7705.h>

void set_cpha1(void)
{
  spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
}

void set_cpol1cpha1(void)
{
  spi_set_format(spi1, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
}

void SystemInit(void)
{
  gpio_init(ADS1259_RESET_PIN);
  gpio_init(ADS1259_START_PIN);
  gpio_init(ADS1259_DRDY_PIN);
  ADS1259_RESET_CLR(0);
  ADS1259_START_CLR(0);
  gpio_set_dir(ADS1259_RESET_PIN, true);
  gpio_set_dir(ADS1259_START_PIN, true);
  gpio_set_dir(ADS1259_DRDY_PIN, false);
  gpio_pull_down(ADS1259_DRDY_PIN);

  gpio_init(ADS1232_GAIN0_PIN);
  gpio_init(ADS1232_GAIN1_PIN);
  gpio_init(ADS1232_A0_PIN);
  gpio_init(ADS1232_TEMP_PIN);
  gpio_set_dir(ADS1232_GAIN0_PIN, true);
  gpio_set_dir(ADS1232_GAIN1_PIN, true);
  gpio_set_dir(ADS1232_A0_PIN, true);
  gpio_set_dir(ADS1232_TEMP_PIN, true);

  spi_init(spi1, 1000);
  gpio_set_function(SPI1_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SPI1_TX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SPI1_RX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SPI1_CS_PIN, GPIO_FUNC_SPI);
}

void delayms(unsigned int ms)
{
  sleep_ms(ms);
}

void ads1259_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength)
{
  unsigned int length = wlength + rlength;
  unsigned char wbuffer[length];
  unsigned char rbuffer[length];
  if (wlength > 0)
    memcpy(wbuffer, wdata, wlength);
  memset(wbuffer + wlength, 0, rlength);
  spi_write_read_blocking(spi1, wbuffer, rbuffer, length);
  if (rlength > 0)
    memcpy(rdata, rbuffer + wlength, rlength);
}

void ad7124_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength)
{
  ads1259_spi_transfer(channel, wdata, wlength, rdata, rlength);
}

void ad7705_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength)
{
  ads1259_spi_transfer(channel, wdata, wlength, rdata, rlength);
}

void ads1232_spi_transfer(int channel, unsigned char *rdata)
{
  spi_read_blocking(spi1, 0, rdata, 3);
}
