#include "board.h"
#include <hardware/gpio.h>
#include <spi_memory.h>

void SystemInit(void)
{
  gpio_init(QSPI_CS_PIN);
  gpio_put(QSPI_CS_PIN, 1);
  gpio_set_dir(QSPI_CS_PIN, GPIO_OUT);

  gpio_init(QSPI_CLK_PIN);
  gpio_put(QSPI_CLK_PIN, 0);
  gpio_set_dir(QSPI_CLK_PIN, GPIO_OUT);

  gpio_init(QSPI_SI_SIO0_PIN);
  gpio_pull_down(QSPI_SI_SIO0_PIN);
  gpio_put(QSPI_SI_SIO0_PIN, 0);
  gpio_set_dir(QSPI_SI_SIO0_PIN, GPIO_OUT);

  gpio_init(QSPI_SO_SIO1_PIN);
  gpio_pull_down(QSPI_SO_SIO1_PIN);
  gpio_set_dir(QSPI_SO_SIO1_PIN, GPIO_IN);

  gpio_init(QSPI_SIO2_PIN);
  gpio_pull_down(QSPI_SIO2_PIN);
  gpio_set_dir(QSPI_SIO2_PIN, GPIO_IN);

  gpio_init(QSPI_SIO3_PIN);
  gpio_pull_down(QSPI_SIO3_PIN);
  gpio_set_dir(QSPI_SIO3_PIN, GPIO_IN);
}

static void spi_delay(void)
{
  for (int i = 0; i < 10; i++)
    asm volatile("nop");
}

void spi_trfr(int channel, int nwrite, const unsigned char *wdata, int nop_cycles, int nread, unsigned char *rdata, int set_cs)
{
  gpio_put(QSPI_CS_PIN, 0);
  while (nwrite--)
  {
    unsigned char data = *wdata++;
    for (int i = 0; i < 8; i++)
    {
      gpio_put(QSPI_SI_SIO0_PIN, data & 0x80 ? true : false);
      spi_delay();
      gpio_put(QSPI_CLK_PIN, true);
      spi_delay();
      gpio_put(QSPI_CLK_PIN, false);
      data <<= 1;
    }
  }
  while (nop_cycles--)
  {
    spi_delay();
    gpio_put(QSPI_CLK_PIN, true);
    spi_delay();
    gpio_put(QSPI_CLK_PIN, false);
  }
  while (nread--)
  {
    unsigned char data = 0;
    for (int i = 0; i < 8; i++)
    {
      spi_delay();
      data <<= 1;
      if (gpio_get(QSPI_SO_SIO1_PIN))
        data |= 1;
      gpio_put(QSPI_CLK_PIN, true);
      spi_delay();
      gpio_put(QSPI_CLK_PIN, false);
    }
    *rdata++ = data;
  }
  if (set_cs)
    gpio_put(QSPI_CS_PIN, 1);
}

void spi_finish(int channel)
{
  gpio_put(QSPI_CS_PIN, 1);
}

void qspi_trfr(int nwrite, const unsigned char *wdata, int nop_cycles, int nread, unsigned char *rdata, int set_cs)
{
  gpio_put(QSPI_CS_PIN, 0);
  gpio_put(QSPI_CS_PIN, 1);
}
