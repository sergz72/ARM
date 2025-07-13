#include "board.h"
#include <pll.h>
#include <gpio.h>
#include <nvic.h>
#include <spi.h>

const RCCConfig rcc_config =
{
  .hse_frequency = 8000000,
  .hsebypass = 0,
  .pllm = 1,
  .pllp_frequency = 240000000,
  .pllq_frequency = 48000000,
  .hpre = 1,
  .ppre1 = 1,
  .ppre2 = 1,
  .ppre3 = 1
};

static const SPI_InitStruct spi1_init = {
  .fifo_threshold = 8,
  .io_swap = 0,
  .alternate_function_gpio_control = 0,
  .comm_mode = SPI_COMM_MODE_SIMPLEX_TX,
  .master = 1,
  .clock_phase = 0,
  .clock_polarity = 0,
  .data_size = 8,
  .dxp_interrupt_enable = 0,
  .rxp_interrupt_enable = 0,
  .txp_interrupt_enable = 0,
  .eot_interrupt_enable = 0,
  .txtf_interrupt_enable = 0,
  .lsb_first = 0,
  .ss_output_enable = 0,
  .ti_mode = 0,
  .tx_dma_enable = 0,
  .rx_dma_enable = 0,
  .software_slave_management = 1,
  .internal_slave_select = 1,
  .baud_rate = 10000000
};

static void GPIOInit(void)
{
  GPIO_InitTypeDef init;

  LED_OFF;
  init.Pin = GPIO_Pin_13;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  // LED
  GPIO_Init(GPIOC, &init);

  init.Pin = GPIO_Pin_0;
  init.Mode = GPIO_MODE_INPUT;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  // BUTTON
  GPIO_Init(GPIOA, &init);
}

static void USB_Init(void)
{
  GPIO_InitTypeDef init;
  //PA11=USBDM,PA12=USBDP
  init.Pin = GPIO_Pin_11 | GPIO_Pin_12;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  init.Alternate = GPIO_AF10_USB;
  GPIO_Init(GPIOA, &init);

  RCC->CCIPR4 |= RCC_CCIPR4_USBSEL_0; // pll1_q_ck selected as usb kernel clock
  RCC->APB2ENR |= RCC_APB2ENR_USBEN;

  NVIC_Init(USB_DRD_FS_IRQn, 1, 3, ENABLE);
}

static void SPIInit(void)
{
  GPIO_InitTypeDef init;

  SPI_LCD_ENABLE;

  init.Pin = SPI_LCD_SCK_PIN;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  init.Alternate = SPI_LCD_SCK_AF;
  GPIO_Init(SPI_LCD_SCK_PORT, &init);

  init.Pin = SPI_LCD_MOSI_PIN;
  init.Alternate = SPI_LCD_MOSI_AF;
  GPIO_Init(SPI_LCD_MOSI_PORT, &init);

  ST7789_CS_PIN_SET;
  ST7789_RST_PIN_SET;
  init.Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(GPIOB, &init);

  SPI_Init(SPI_LCD, &spi1_init);
  SPI_Enable(SPI_LCD);
}

void SystemInit(void)
{
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;

  GPIOInit();

  if (InitRCC(&rcc_config))
  {
    LED_ON;
    while (1)
      ;
  }

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  USB_Init();
  SPIInit();
}

void _init(void)
{

}

void ST7789_WriteBytes(unsigned char *data, unsigned int size)
{
  while (size--)
    SPI_Send8(SPI_LCD, *data++, SPI_TIMEOUT);
  SPI1->CR1 |= SPI_CR1_CSTART;
  SPI_WaitSend(SPI_LCD, SPI_TIMEOUT);
}

void __attribute__((section(".RamFunc"))) set_h_voltage(unsigned int value)
{
  DAC1->DHR12R2 = mv_to_12(value);
}

void __attribute__((section(".RamFunc"))) set_l_voltage(unsigned int value)
{
  DAC1->DHR12R1 = mv_to_12(value);
}

unsigned int get_l_voltage(void)
{
  return mv_from_12(DAC1->DHR12R1);
}

unsigned int get_h_voltage(void)
{
  return mv_from_12(DAC1->DHR12R2);
}

void __attribute__((section(".RamFunc"))) stop_counters(void)
{
  //todo
}

void __attribute__((section(".RamFunc"))) start_counters(void)
{
  //todo
}

void pwm_set_frequency_and_duty(unsigned int frequency, unsigned int duty)
{
  //todo
  /*unsigned int arr = BOARD_PCLK1 / frequency;
  if (arr < 2)
    arr = 2;
  if (arr > 65536)
    arr = 65536;
  if (duty == 0)
    duty = 1;
  else if (duty > 99)
    duty = 99;
  TIM17->ARR = arr - 1;
  TIM17->CCR1 = arr * duty / 100;*/
}

void adc_start(void)
{
}

unsigned int get_adc_voltage(void)
{
  return 0;
}

void connect_pullup(void)
{
}

void disconnect_pullup(void)
{
}
