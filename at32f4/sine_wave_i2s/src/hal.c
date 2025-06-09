#include "board.h"
#include "at32f403a_407_gpio.h"
#include "at32f403a_407_crm.h"
#include "at32f403a_407_i2c.h"
#include "at32f403a_407_misc.h"
#include <tlv320dac3100.h>
#include <i2c_func.h>
#include "sound.h"

// hext = 8 MHz, pll clock is 168 mhz
static void CLOCKInit(void)
{
  /* reset crm */
  crm_reset();

  crm_clock_source_enable(CRM_CLOCK_SOURCE_HEXT, TRUE);

  /* wait till hext is ready */
  while(crm_hext_stable_wait() == ERROR)
  {
  }

  /* config pll clock resource */
  crm_pll_config(CRM_PLL_SOURCE_HEXT, CRM_PLL_MULT_21, CRM_PLL_OUTPUT_RANGE_GT72MHZ);

  /* config hext division */
  //crm_hext_clock_div_set(CRM_HEXT_DIV_2);

  /* enable pll */
  crm_clock_source_enable(CRM_CLOCK_SOURCE_PLL, TRUE);

  /* wait till pll is ready */
  while(crm_flag_get(CRM_PLL_STABLE_FLAG) != SET)
  {
  }

  /* config ahbclk = 84 MHz */
  crm_ahb_div_set(CRM_AHB_DIV_2);

  /* config apb2clk, the maximum frequency of APB1/APB2 clock is 120 MHz */
  crm_apb2_div_set(CRM_APB2_DIV_1);

  /* config apb1clk, the maximum frequency of APB1/APB2 clock is 120 MHz  */
  crm_apb1_div_set(CRM_APB1_DIV_1);

  /* enable auto step mode */
  crm_auto_step_mode_enable(TRUE);

  /* select pll as system clock source */
  crm_sysclk_switch(CRM_SCLK_PLL);

  /* wait till pll is used as system clock source */
  while(crm_sysclk_switch_status_get() != CRM_SCLK_PLL)
  {
  }

  /* disable auto step mode */
  crm_auto_step_mode_enable(FALSE);

  /* update system_core_clock global variable */
  system_core_clock_update();
}

static void GPIOInit(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the led clock */
  crm_periph_clock_enable(LED_CLOCK, TRUE);

  LED_OFF;
  /* configure the gpio */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = LED_PIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(LED_PORT, &gpio_init_struct);
}

/*
  SPI1-I2S1:
     WS -- PA15
     CK -- PB3
     SD -- PB5
     MCK-- PB6
*/

static void I2SInit(void)
{
  gpio_init_type gpio_init_struct;
  i2s_init_type i2s_init_struct;

  crm_periph_clock_enable(I2S_CLK, TRUE);
  crm_periph_clock_enable(I2S_WS_GPIO_CLK, TRUE);
  crm_periph_clock_enable(I2S_CK_GPIO_CLK, TRUE);
  crm_periph_clock_enable(I2S_SD_GPIO_CLK, TRUE);
  crm_periph_clock_enable(I2S_MCK_GPIO_CLK, TRUE);

#ifdef I2S_MUX
  gpio_pin_remap_config(I2S_MUX, TRUE);
#endif

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type       = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull           = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode           = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins           = I2S_WS_PIN;
  gpio_init(I2S_WS_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins           = I2S_CK_PIN;
  gpio_init(I2S_CK_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins           = I2S_SD_PIN;
  gpio_init(I2S_SD_GPIO_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins           = I2S_MCK_PIN;
  gpio_init(I2S_MCK_GPIO_PORT, &gpio_init_struct);

  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_PHILLIPS;
  i2s_init_struct.data_channel_format = I2S_DATA_16BIT_CHANNEL_16BIT;
  i2s_init_struct.mclk_output_enable = TRUE;
  i2s_init_struct.audio_sampling_freq = I2S_AUDIO_FREQUENCY_16K;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = I2S_MODE_MASTER_TX;
  i2s_init(I2S_INSTANCE, &i2s_init_struct);

  spi_i2s_dma_transmitter_enable(I2S_INSTANCE, TRUE);
  i2s_enable(I2S_INSTANCE, TRUE);
}

void DMA_I2S_Init(void)
{
  dma_init_type dma_init_struct;

  crm_periph_clock_enable(I2S_DMA_CLK, TRUE);

  dma_init_struct.buffer_size = SOUND_OUT_BUFFER_SIZE << 1;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)sound_out_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)I2S_DT_ADDRESS;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(I2S_DMA_CHANNEL, &dma_init_struct);
  dma_interrupt_enable(I2S_DMA_CHANNEL, DMA_FDT_INT, TRUE);
  dma_interrupt_enable(I2S_DMA_CHANNEL, DMA_HDT_INT, TRUE);
  nvic_irq_enable(I2S_DMA_IRQ, 1, 0);
}

static void I2CInit(void)
{
  gpio_init_type gpio_initstructure;

  crm_periph_clock_enable(I2C_CLK, TRUE);
#ifdef I2C_MUX
  gpio_pin_remap_config(I2C_MUX, TRUE);
#endif
  crm_periph_clock_enable(I2C_SCL_GPIO_CLK, TRUE);
  crm_periph_clock_enable(I2C_SDA_GPIO_CLK, TRUE);

  /* gpio configuration */
  gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_initstructure.gpio_pull           = GPIO_PULL_NONE;
  gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;

  /* configure i2c pins: scl */
  gpio_initstructure.gpio_pins = I2C_SCL_PIN;
  gpio_init(I2C_SCL_GPIO_PORT, &gpio_initstructure);

  /* configure i2c pins: sda */
  gpio_initstructure.gpio_pins = I2C_SDA_PIN;
  gpio_init(I2C_SDA_GPIO_PORT, &gpio_initstructure);

  i2c_init(I2C_INSTANCE, I2C_FSMODE_DUTY_2_1, I2C_SPEED);

  i2c_own_address1_set(I2C_INSTANCE, I2C_ADDRESS_MODE_7BIT, 0);

  i2c_enable(I2C_INSTANCE, TRUE);
}

static void TLVInit(void)
{
  gpio_init_type gpio_init_struct;

  // TLV reset pin
  crm_periph_clock_enable(TLV_RESET_CLOCK, TRUE);

  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = TLV_RESET_PIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(TLV_RESET_PORT, &gpio_init_struct);
}

void HALInit(void)
{
  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);

  gpio_pin_remap_config(SWJTAG_MUX_010, TRUE);

  CLOCKInit();
  GPIOInit();
  I2SInit();
  DMA_I2S_Init();
  I2CInit();
  TLVInit();
}

int tlv320dac3100_read(unsigned char register_number, unsigned char *value)
{
  return 1;
}

int tlv320dac3100_write(unsigned char register_number, unsigned char value)
{
  unsigned char data[2];
  data[0] = register_number;
  data[1] = value;
  return i2c_master_transmit(I2C_INSTANCE, TLV320DAC3100_I2C_ADDRESS, data, 2, I2C_TIMEOUT);
}
