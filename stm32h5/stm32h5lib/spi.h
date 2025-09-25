#ifndef SPI_H
#define SPI_H

#define SPI_COMM_MODE_FULLDUPLEX 0
#define SPI_COMM_MODE_SIMPLEX_TX (1 << 17)
#define SPI_COMM_MODE_SIMPLEX_RX (2 << 17)
#define SPI_COMM_MODE_HALFDUPLEX (3 << 17)

typedef struct
{
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

#define I2S_MODE_SLAVE_TX 0
#define I2S_MODE_SLAVE_RX (1<<8)
#define I2S_MODE_MASTER_TX (2<<8)
#define I2S_MODE_MASTER_RX (3<<8)

#define I2S_STANDARD_PHILIPS 0
#define I2S_STANDARD_MSB (1<<4)
#define I2S_STANDARD_LSB (2<<4)
#define I2S_STANDARD_PCM (3<<4)

#define I2S_DATA_SIZE_16BIT 0
#define I2S_DATA_SIZE_24BIT 2
#define I2S_DATA_SIZE_32BIT 4

#define I2S_CONFIGURATION_SLAVE_TX  0
#define I2S_CONFIGURATION_SLAVE_RX  2
#define I2S_CONFIGURATION_MASTER_TX 4
#define I2S_CONFIGURATION_MASTER_RX 6
#define I2S_CONFIGURATION_SLAVE     8
#define I2S_CONFIGURATION_MASTER    10

typedef struct
{
  unsigned int mode;
  int long_frame_sync;
  unsigned int standard;
  int clock_polarity;
  unsigned int data_size;
  int channel_32bit;
  int master_clock_output_enable;
  unsigned int i2sdiv;
  unsigned int configuration;
} I2S_InitStruct;

void SPI_Init(SPI_TypeDef *instance, const SPI_InitStruct *init);
void SPI_Enable(SPI_TypeDef *instance);
void SPI_Disable(SPI_TypeDef *instance);
int SPI_Send8(SPI_TypeDef *instance, unsigned char data, unsigned int timeout);
int SPI_Send16(SPI_TypeDef *instance, unsigned short data, unsigned int timeout);
int SPI_WaitSend(SPI_TypeDef *instance, unsigned int timeout);

void I2S_Init(SPI_TypeDef *instance, const I2S_InitStruct *init);
void I2S_Enable(SPI_TypeDef *instance);
void I2S_Disable(SPI_TypeDef *instance);

#endif
