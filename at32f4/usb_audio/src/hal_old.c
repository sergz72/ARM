#include "board.h"
#include <at32f403a_407_i2c.h>

/**
* @brief  initializes peripherals used by the i2c.
  * @param  none
  * @retval none
  */
void i2c_lowlevel_init(i2c_handle_type* hi2c)
{
  gpio_init_type gpio_initstructure;

  if(hi2c->i2cx == I2Cx_PORT)
  {
    /* i2c periph clock enable */
    crm_periph_clock_enable(I2Cx_CLK, TRUE);
    crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
    gpio_pin_remap_config(I2C1_MUX, TRUE);
    crm_periph_clock_enable(I2Cx_SCL_GPIO_CLK, TRUE);
    crm_periph_clock_enable(I2Cx_SDA_GPIO_CLK, TRUE);

    /* gpio configuration */
    gpio_initstructure.gpio_out_type       = GPIO_OUTPUT_OPEN_DRAIN;
    gpio_initstructure.gpio_pull           = GPIO_PULL_UP;
    gpio_initstructure.gpio_mode           = GPIO_MODE_MUX;
    gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;

    /* configure i2c pins: scl */
    gpio_initstructure.gpio_pins = I2Cx_SCL_PIN;
    gpio_init(I2Cx_SCL_GPIO_PORT, &gpio_initstructure);

    /* configure i2c pins: sda */
    gpio_initstructure.gpio_pins = I2Cx_SDA_PIN;
    gpio_init(I2Cx_SDA_GPIO_PORT, &gpio_initstructure);

    i2c_init(hi2c->i2cx, I2C_FSMODE_DUTY_2_1, I2Cx_SPEED);

    i2c_own_address1_set(hi2c->i2cx, I2C_ADDRESS_MODE_7BIT, I2Cx_ADDRESS);
  }
}


/**
  * @brief  mclk clock conifg
  * @param  none
  * @retval none
  */
void mclk_tmr1_init(void)
{
  gpio_init_type gpio_init_struct;
  tmr_output_config_type tmr_oc_init_structure;
  uint16_t prescaler_value = (uint16_t)(system_core_clock / 24000000) - 1;

  crm_periph_clock_enable(CRM_TMR1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(GPIOA, &gpio_init_struct);

  tmr_base_init(TMR1, 1, prescaler_value);
  tmr_cnt_dir_set(TMR1, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR1, TMR_CLOCK_DIV1);

  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = TRUE;
  tmr_output_channel_config(TMR1, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR1, TMR_SELECT_CHANNEL_1, 1);
  tmr_output_channel_buffer_enable(TMR1, TMR_SELECT_CHANNEL_1, TRUE);

  /* tmr enable counter */
  tmr_counter_enable(TMR1, TRUE);
  tmr_output_enable(TMR1, TRUE);
}

/**
  * @brief  i2s reset
  * @param  none
  * @retval none
  */
void i2s_reset(void)
{
  i2s_enable(SPI1, FALSE);
  i2s_enable(SPI2, FALSE);
  dma_channel_enable(DMA1_CHANNEL3, FALSE);
  dma_channel_enable(DMA1_CHANNEL4, FALSE);
}

void i2s_init(audio_codec_type *param)
{
  gpio_init_type gpio_init_struct;
  dma_init_type dma_init_struct;
  i2s_init_type i2s_init_struct;
  i2s_data_channel_format_type format;

  if(param->audio_bitw == 16)
  {
    format = I2S_DATA_16BIT_CHANNEL_16BIT;
  }
  else
  {
    format = I2S_DATA_24BIT_CHANNEL_32BIT;
  }

  crm_periph_clock_enable(I2S1_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(I2S2_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* i2s1 ws pins */
  gpio_init_struct.gpio_pull           = GPIO_PULL_UP;
  gpio_init_struct.gpio_mode           = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins           = I2S1_WS_PIN;
  gpio_init(I2S1_GPIO, &gpio_init_struct);

  /* i2s1 ck pins */
  gpio_init_struct.gpio_pull           = GPIO_PULL_DOWN;
  gpio_init_struct.gpio_mode           = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins           = I2S1_CK_PIN;
  gpio_init(I2S1_GPIO, &gpio_init_struct);

  /* i2s1 sd pins slave tx */
  gpio_init_struct.gpio_pull           = GPIO_PULL_UP;
  gpio_init_struct.gpio_mode           = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins           = I2S1_SD_PIN;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
  gpio_init(I2S1_GPIO, &gpio_init_struct);

  /* i2s2 ws pins */
  gpio_init_struct.gpio_pull           = GPIO_PULL_UP;
  gpio_init_struct.gpio_mode           = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins           = I2S2_WS_PIN;
  gpio_init(I2S2_GPIO, &gpio_init_struct);

  /* i2s2 ck pins */
  gpio_init_struct.gpio_pull           = GPIO_PULL_DOWN;
  gpio_init_struct.gpio_mode           = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins           = I2S2_CK_PIN;
  gpio_init(I2S2_GPIO, &gpio_init_struct);

  /* i2s2 sd pins slave rx */
  gpio_init_struct.gpio_pull           = GPIO_PULL_UP;
  gpio_init_struct.gpio_mode           = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins           = I2S2_SD_PIN;
  gpio_init(I2S2_GPIO, &gpio_init_struct);

  /* dma config */
  dma_reset(DMA1_CHANNEL3);
  dma_reset(DMA1_CHANNEL4);

  /* dma1 channel3: speaker i2s1 tx */
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = param->spk_tx_size << 1;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)spk_dma_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)I2S1_DT_ADDRESS;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA1_CHANNEL3, &dma_init_struct);
  dma_interrupt_enable(DMA1_CHANNEL3, DMA_FDT_INT, TRUE);
  dma_interrupt_enable(DMA1_CHANNEL3, DMA_HDT_INT, TRUE);
  nvic_irq_enable(DMA1_Channel3_IRQn, 1, 0);

  /* dma1 channel4: microphone i2s2 rx */
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = param->mic_rx_size << 1;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)mic_dma_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)I2S2_DT_ADDRESS;
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA1_CHANNEL4, &dma_init_struct);
  dma_interrupt_enable(DMA1_CHANNEL4, DMA_FDT_INT, TRUE);
  dma_interrupt_enable(DMA1_CHANNEL4, DMA_HDT_INT, TRUE);
  nvic_irq_enable(DMA1_Channel4_IRQn, 2, 0);

  /* i2s1 tx init */
  spi_i2s_reset(SPI1);
  i2s_default_para_init(&i2s_init_struct);
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_PHILLIPS;
  i2s_init_struct.data_channel_format = format;
  i2s_init_struct.mclk_output_enable = FALSE;
  i2s_init_struct.audio_sampling_freq = (i2s_audio_sampling_freq_type)param->audio_freq;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = I2S_MODE_SLAVE_TX;
  i2s_init(SPI1, &i2s_init_struct);

   /* i2s2 rx init */
  spi_i2s_reset(SPI2);
  i2s_default_para_init(&i2s_init_struct);
  i2s_init_struct.audio_protocol = I2S_AUDIO_PROTOCOL_PHILLIPS;
  i2s_init_struct.data_channel_format = format;
  i2s_init_struct.mclk_output_enable = FALSE;
  i2s_init_struct.audio_sampling_freq = (i2s_audio_sampling_freq_type)param->audio_freq;
  i2s_init_struct.clock_polarity = I2S_CLOCK_POLARITY_LOW;
  i2s_init_struct.operation_mode = I2S_MODE_SLAVE_RX;
  i2s_init(SPI2, &i2s_init_struct);


  spi_i2s_dma_transmitter_enable(SPI1, TRUE);
  spi_i2s_dma_receiver_enable(SPI2, TRUE);
  i2s_enable(SPI1, TRUE);
  i2s_enable(SPI2, TRUE);

  dma_channel_enable(DMA1_CHANNEL3, TRUE);
  dma_channel_enable(DMA1_CHANNEL4, TRUE);
}