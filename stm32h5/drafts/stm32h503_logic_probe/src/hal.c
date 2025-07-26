#include "board.h"
#include <pll.h>
#include <gpio.h>
#include <nvic.h>
#include <spi.h>
#include <lcd_st7789.h>

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

static const SPI_InitStruct spi_init = {
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

static unsigned int prev_flags = ST7789_FLAG_CS;

static void GPIOInit(void)
{
  GPIO_InitTypeDef init;

  LED_OFF;
  init.Pin = GPIO_Pin_13;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_PULLDOWN;
  // LED
  GPIO_Init(GPIOC, &init);

  init.Pin = GPIO_Pin_1 | GPIO_Pin_2;
  init.Mode = GPIO_MODE_INPUT;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_PULLDOWN;
  // BUTTONs
  GPIO_Init(GPIOB, &init);
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

/*
 *PA1=SPI3_SCK
 *PA3=SPI3_MOSI
 *PA6=RESET
 *PA7=DC
 *PB0=CS
 */
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
  init.Pin = GPIO_Pin_6 | GPIO_Pin_7;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(GPIOA, &init);
  init.Pin = GPIO_Pin_0;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(GPIOB, &init);

  SPI_Init(SPI_LCD, &spi_init);
  SPI_Enable(SPI_LCD);
}

// PA4=DAC1_OUT1, PA5=DAC1_OUT2
static void DACInit(void)
{
  RCC->AHB2ENR |= RCC_AHB2ENR_DAC1EN;

  GPIO_InitTypeDef init;
  init.Pin = GPIO_Pin_4 | GPIO_Pin_5;
  init.Mode = GPIO_MODE_ANALOG;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(GPIOA, &init);

  DAC1->DHR12R1 = mv_to_12(DEFAULT_DACL_VOLTAGE);
  DAC1->DHR12R2 = mv_to_12(DEFAULT_DACH_VOLTAGE);
  //DAC channels 1,2 are connected to external pins with Buffer enabled
  //DAC1->MCR = 0;
  DAC1->CR = DAC_CR_EN1 | DAC_CR_EN2;
}

// GATE timer
static void TIM6Init(void)
{
  RCC->APB1LENR |= RCC_APB1LENR_TIM6EN;

  TIM6->PSC = BOARD_PCLK1 / 100000 - 1;
  TIM6->ARR = 100000 / TIMER_EVENT_FREQUENCY - 1;
  TIM6->DIER = TIM_DIER_UIE;

  TIM6->SR = 0;

  NVIC_Init(TIM6_IRQn, TIMER_INTERRUPT_PRIORITY, 0, ENABLE);
}

void GatedModeSet(TIM_TypeDef *tim)
{
  // Configure in gated mode
  tim->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_2 |                  // Gated mode
               TIM_SMCR_TS_0 | TIM_SMCR_TS_1 | TIM_SMCR_TS_2;    // tim_etrf as trigger source
}

void ExternalClockModeSet(TIM_TypeDef *tim)
{
  // Configure TIM in external clock mode
  tim->SMCR = TIM_SMCR_SMS_0 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_2 |
                TIM_SMCR_TS_0 | TIM_SMCR_TS_1 | TIM_SMCR_TS_2;    // tim_etrf as trigger source
}

// low counter, PA0 = TIM2_ETR
static void TIM2Init(void)
{
  GPIO_InitTypeDef init;

  RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;

  init.Pin = GPIO_Pin_0;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_PULLDOWN;
  init.Alternate = GPIO_AF14_TIM2;
  GPIO_Init(GPIOA, &init);

  GatedModeSet(TIM2);
}

// high counter, PA2 = TIM3_ETR
static void TIM3Init(void)
{
  GPIO_InitTypeDef init;

  RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;

  init.Pin = GPIO_Pin_2;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_PULLDOWN;
  init.Alternate = GPIO_AF2_TIM3;
  GPIO_Init(GPIOA, &init);

  GatedModeSet(TIM3);

  TIM3->DIER = TIM_DIER_UIE;

  TIM3->SR = 0;
  NVIC_Init(TIM3_IRQn, COUNTERS_INTERRUPT_PRIORITY, 0, ENABLE);
}

// PWM timer, PB5=TIM1_CH3
static void TIM1Init(void)
{
  GPIO_InitTypeDef init;

  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  init.Pin = GPIO_Pin_5;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_PULLDOWN;
  init.Alternate = GPIO_AF1_TIM1;
  GPIO_Init(GPIOB, &init);

  pwm_set_frequency_and_duty(10000, 50);

  TIM1->CCMR2 = (6 << TIM_CCMR2_OC3M_Pos) | TIM_CCMR2_OC3PE;  // PWM mode 1 (0110)

  TIM1->CCER |= TIM_CCER_CC3E;

  TIM1->BDTR = TIM_BDTR_MOE;

  TIM1->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
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
  DACInit();
  TIM6Init();
  TIM2Init();
  TIM3Init();
  TIM1Init();
}

void _init(void)
{

}

void ST7789_WriteBytes(unsigned int flags, unsigned char *data, unsigned int size)
{
  if (prev_flags != flags)
  {
    prev_flags = flags;
    if (flags & ST7789_FLAG_DC)
      ST7789_DC_PIN_SET;
    else
      ST7789_DC_PIN_CLR;
    if (flags & ST7789_FLAG_CS)
    {
      ST7789_CS_PIN_SET;
      return;
    }
    ST7789_CS_PIN_CLR;
  }
  while (size--)
    SPI_Send8(SPI_LCD, *data++, SPI_TIMEOUT);
  SPI_LCD->CR1 |= SPI_CR1_CSTART;
  SPI_WaitSend(SPI_LCD, SPI_TIMEOUT);
}

void ST7789_WriteColor(unsigned int color, unsigned int count)
{
  prev_flags = ST7789_FLAG_DC;
  ST7789_DC_PIN_SET;
  while (count--)
  {
    SPI_Send8(SPI_LCD, (unsigned char)color, SPI_TIMEOUT);
    SPI_Send8(SPI_LCD, (unsigned char)(color >> 8), SPI_TIMEOUT);
    SPI_LCD->CR1 |= SPI_CR1_CSTART;
    SPI_WaitSend(SPI_LCD, SPI_TIMEOUT);
  }
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
  TIM6->CR1 &= ~TIM_CR1_CEN;
  TIM2->CR1 &= ~TIM_CR1_CEN;
  TIM3->CR1 &= ~TIM_CR1_CEN;
}

void __attribute__((section(".RamFunc"))) start_counters(void)
{
  TIM6->CNT = 0;
  TIM2->CNT = 0;
  TIM3->CNT = 0;
  TIM6->CR1 |= TIM_CR1_CEN;
  TIM2->CR1 |= TIM_CR1_CEN;
  TIM3->CR1 |= TIM_CR1_CEN;
}

void pwm_set_frequency_and_duty(unsigned int frequency, unsigned int duty)
{
  unsigned int arr = BOARD_PCLK1 / frequency;
  if (arr < 2)
    arr = 2;
  if (arr > 65536)
    arr = 65536;
  if (duty == 0)
    duty = 1;
  else if (duty > 99)
    duty = 99;
  TIM1->ARR = arr - 1;
  TIM1->CCR3 = arr * duty / 100;
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
