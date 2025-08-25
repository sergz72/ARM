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

  spi_memory_init();
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

void qspi_set_sio_direction(int out0, int out1, int out2, int out3)
{
  gpio_set_dir(QSPI_SI_SIO0_PIN, out0);
  gpio_set_dir(QSPI_SO_SIO1_PIN, out1);
  gpio_set_dir(QSPI_SIO2_PIN, out2);
  gpio_set_dir(QSPI_SIO3_PIN, out3);
}

static void qspi_data_set(unsigned char data)
{
  gpio_put(QSPI_SI_SIO0_PIN, data & 1 ? true : false);
  gpio_put(QSPI_SO_SIO1_PIN, data & 2 ? true : false);
  gpio_put(QSPI_SIO2_PIN, data & 4 ? true : false);
  gpio_put(QSPI_SIO3_PIN, data & 8 ? true : false);
}

static unsigned char qspi_data_get(void)
{
  unsigned int value32 = gpio_get_all();
  unsigned char value = value32 & (1<<QSPI_SI_SIO0_PIN) ? 1 : 0;
  if (value32 & (1<<QSPI_SO_SIO1_PIN))
    value |= 2;
  if (value32 & (1<<QSPI_SIO2_PIN))
    value |= 4;
  if (value32 & (1<<QSPI_SIO3_PIN))
    value |= 8;
  return value;
}

void qspi_trfr(int channel, int nwrite, const unsigned char *wdata, int nop_cycles, int nread, unsigned char *rdata, int set_cs)
{
  gpio_put(QSPI_CS_PIN, 0);

  while (nwrite--)
  {
    unsigned char data = *wdata++;
    qspi_data_set(data >> 4);
    spi_delay();
    gpio_put(QSPI_CLK_PIN, true);
    spi_delay();
    gpio_put(QSPI_CLK_PIN, false);
    qspi_data_set(data);
    spi_delay();
    gpio_put(QSPI_CLK_PIN, true);
    spi_delay();
    gpio_put(QSPI_CLK_PIN, false);
  }
  if (nread)
  {
    qspi_set_sio_direction(GPIO_IN, GPIO_IN, GPIO_IN, GPIO_IN);
    while (nop_cycles--)
    {
      spi_delay();
      gpio_put(QSPI_CLK_PIN, true);
      spi_delay();
      gpio_put(QSPI_CLK_PIN, false);
    }
    while (nread--)
    {
      spi_delay();
      gpio_put(QSPI_CLK_PIN, true);
      unsigned char value = qspi_data_get() << 4;
      spi_delay();
      gpio_put(QSPI_CLK_PIN, false);
      spi_delay();
      gpio_put(QSPI_CLK_PIN, true);
      value |= qspi_data_get();
      *rdata++ = value;
      spi_delay();
      gpio_put(QSPI_CLK_PIN, false);
    }
  }
  if (set_cs)
  {
    gpio_put(QSPI_CS_PIN, 1);
    qspi_set_sio_direction(GPIO_OUT, GPIO_OUT, GPIO_OUT, GPIO_OUT);
  }
}
