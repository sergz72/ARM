#include "board.h"
#include <hardware/gpio.h>
#include <hardware/spi.h>
#include <cc1101.h>
#include <string.h>

#include "pico/time.h"

static void SPIInit(void)
{
  spi_init(SPI_INST, SPI_BAUD_RATE);
  gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SPI_TX_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SPI_RX_PIN, GPIO_FUNC_SPI);
  //gpio_set_function(SPI_CS_PIN, GPIO_FUNC_SPI);
  gpio_init(SPI_CS_PIN);
  cc1101_CSN_SET();
  gpio_set_dir(SPI_CS_PIN, true);
}

static void CC1101Init(void)
{
  gpio_init(CC1101_GD0_PIN);
  gpio_set_dir(CC1101_GD0_PIN, false);
  gpio_init(CC1101_GD2_PIN);
  gpio_set_dir(CC1101_GD2_PIN, false);
}

void SystemInit(void)
{
  SPIInit();
  CC1101Init();
}

void delay(unsigned int us)
{
  sleep_us(us);
}

int cc1101_RW(unsigned int device_num, unsigned char *txdata, unsigned char *rxdata, unsigned int size)
{
  int rc;

  if (size < 2)
    return 0;

  rc = CC1101_TIMEOUT;
  while (--rc && cc1101_GD2) // waiting for chip ready
    ;
  if (!rc)
    return 1; // timeout

  if (size > 2)
    txdata[0] |= CC1101_BURST;

  cc1101_CSN_CLR(device_num);

  spi_write_read_blocking(SPI_INST, txdata, rxdata, size);

  cc1101_CSN_SET(device_num);

  return 0;
}

int cc1101_strobe(unsigned int device_num, unsigned char data, unsigned char *status)
{
  int rc;

  rc = CC1101_TIMEOUT;
  while (--rc && cc1101_GD2) // waiting for chip ready
    ;
  if (!rc)
    return 1; // timeout

  cc1101_CSN_CLR(device_num);

  spi_write_read_blocking(SPI_INST, &data, status, 1);

  cc1101_CSN_SET(device_num);

  return 0;
}
