#include <hardware/clocks.h>

#include "board.h"
#include "hardware/gpio.h"
#include "spi.pio.h"

void SystemInit(void)
{
  gpio_init(PIN_NCS);
  gpio_put(PIN_NCS, true);
  gpio_set_dir(PIN_NCS, GPIO_OUT);
  unsigned int offset = pio_add_program(SPI_PIO, &spi_cpha0_program);
  pio_spi_init(SPI_PIO, SPI_SM, offset, 8, 256, PIN_SCK, PIN_MOSI, PIN_MISO);
}

int spi_set_baudrate(unsigned int value)
{
  unsigned int sys_clk = clock_get_hz(clk_sys);
  unsigned int divider = sys_clk / value;
  if (divider == 0 || divider > 65535)
    return -1;
  pio_sm_set_clkdiv(SPI_PIO, SPI_SM, divider);
  return sys_clk / divider;
}

void __time_critical_func(spi_transfer)(const unsigned char *data, unsigned int num_bytes, unsigned char *rdata)
{
  gpio_put(PIN_NCS, false);
  unsigned int tx_remain = num_bytes, rx_remain = num_bytes;
  volatile unsigned char *txfifo = (volatile unsigned char *) &SPI_PIO->txf[SPI_SM];
  volatile unsigned char *rxfifo = (volatile unsigned char *) &SPI_PIO->rxf[SPI_SM];
  while (tx_remain || rx_remain) {
    if (tx_remain && !pio_sm_is_tx_fifo_full(SPI_PIO, SPI_SM)) {
      *txfifo = *data++;
      tx_remain--;
    }
    if (rx_remain && !pio_sm_is_rx_fifo_empty(SPI_PIO, SPI_SM)) {
      *rdata++ =  *rxfifo;
      rx_remain--;
    }
  }
  gpio_put(PIN_NCS, true);
}
