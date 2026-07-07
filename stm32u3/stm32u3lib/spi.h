#ifndef SPI_H
#define SPI_H

#define SPI_COMM_MODE_FULLDUPLEX 0
#define SPI_COMM_MODE_SIMPLEX_TX (1 << 17)
#define SPI_COMM_MODE_SIMPLEX_RX (2 << 17)
#define SPI_COMM_MODE_HALFDUPLEX (3 << 17)

typedef struct
{
  unsigned int spi_clock;
  int io_swap;
  int alternate_function_gpio_control;
  unsigned int comm_mode;
  int master;
  unsigned int data_size;
  unsigned int fifo_threshold;
  int lsb_first;
  unsigned int baud_rate;
  int clock_polarity;
  int clock_phase;
  int rxp_interrupt_enable;
  int txp_interrupt_enable;
  int dxp_interrupt_enable;
  int eot_interrupt_enable;
  int txtf_interrupt_enable;
  int ti_mode;
  int ss_output_enable;
  int tx_dma_enable;
  int rx_dma_enable;
  int software_slave_management;
  int internal_slave_select;
} SPI_InitStruct;

void SPI_Init(SPI_TypeDef *instance, const SPI_InitStruct *init);
void SPI_Enable(SPI_TypeDef *instance);
void SPI_Disable(SPI_TypeDef *instance);
int SPI_Send8(SPI_TypeDef *instance, unsigned char data, unsigned int timeout);
int SPI_Send16(SPI_TypeDef *instance, unsigned short data, unsigned int timeout);
int SPI_WaitSend(SPI_TypeDef *instance, unsigned int timeout);
int SPI_TransmitReceive(SPI_TypeDef *instance, const unsigned char *txdata, unsigned char *rxdata, unsigned int size, unsigned int timeout);

#endif
