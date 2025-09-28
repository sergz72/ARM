#include "board.h"
#include <pll.h>
#include <nvic.h>
#include <lcd_sh1107.h>
#include <spi.h>
#include <spi_soft.h>
#include <ad7793.h>
#include <ads1220.h>
#include <string.h>

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
  .clock_phase = 1,
  .clock_polarity = 1,
  .data_size = 8,
  .dxp_interrupt_enable = 0,
  .rxp_interrupt_enable = 0,
  .txp_interrupt_enable = 0,
  .eot_interrupt_enable = 0,
  .txtf_interrupt_enable = 0,
  .lsb_first = 0,
  .ss_output_enable = 1,
  .ti_mode = 0,
  .tx_dma_enable = 0,
  .rx_dma_enable = 0,
  .software_slave_management = 0,
  .internal_slave_select = 0,
  .baud_rate = 2000000
};

volatile int capacity_measurement_done;

static void GPIOInit(void)
{
  GPIO_InitTypeDef init;

  POWER_ON;
  init.Pin = ON_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  // POWER ON PIN
  GPIO_Init(ON_PORT, &init);

  LED_OFF;
  init.Pin = LED_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  // LED
  GPIO_Init(LED_PORT, &init);

  init.Pin = BUTTON_OFF_PIN | BUTTON1_PIN | BUTTON2_PIN | BUTTON3_PIN;
  init.Mode = GPIO_MODE_INPUT;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_PULLUP;
  // BUTTONS
  GPIO_Init(BUTTONS_PORT, &init);

  //clr discharge pin
  R_PORT->BSRR = R_DISCHARGE_PIN<<16;
  //set charge1 & charge2 pins
  R_PORT->BSRR = R_CHARGE1_PIN | R_CHARGE2_PIN;
  charge_off();
  discharge_on();
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

static void TIM2Init(void)
{
  GPIO_InitTypeDef init;

  RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;

  // input capture pins: PA3=CH4, PA5=CH1
  init.Pin = CAPTURE_PIN_CHARGE1 | CAPTURE_PIN_CHARGE2 | CAPTURE_PIN_CX;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  init.Alternate = GPIO_AF1_TIM2;
  GPIO_Init(CAPTURE_PORT, &init);

  TIM2->PSC = 0;
  TIM2->ARR = 0xFFFFFFFF;
  //no filter, no prescaler, CC1 channel is configured as input, tim_ic1 is mapped on tim_ti1
  TIM2->CCMR1 = TIM_CCMR1_CC1S_0;
  //no filter, no prescaler, CC4 channel is configured as input, tim_ic4 is mapped on tim_ti4
  TIM2->CCMR2 = TIM_CCMR2_CC4S_0;
  //rising-edge capture mode for all channels
  TIM2->CCER = TIM_CCER_CC1E | TIM_CCER_CC4E;

  NVIC_Init(TIM2_IRQn, TIMER_INTERRUPT_PRIORITY, 0, ENABLE);
}

static void SPI2Init(void)
{
  GPIO_InitTypeDef init;

  RCC->APB1LENR |= RCC_APB1LENR_SPI2EN;

  init.Pin = LCD_SCK_PIN | LCD_DIN_PIN | LCD_CS_PIN;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  init.Alternate = GPIO_AF5_SPI2;
  GPIO_Init(LCD_PORT, &init);

  LCD_RST_CLR;
  init.Pin = LCD_RST_PIN | LCD_DC_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(LCD_PORT, &init);

  SPI_Init(LCD_SPI_INSTANCE, &spi_init);
  SPI_Enable(LCD_SPI_INSTANCE);
}

static void ADS1220_SPI_Init(void)
{
  GPIO_InitTypeDef init;

  spi_channel_init(ADS1220_CHANNEL, 1);

  SPI_CLK_IDLE(ADS1220_CHANNEL);
  init.Pin = ADS1220_CS_PIN | ADS1220_MOSI_PIN | ADS1220_SCK_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(ADS1220_PORT, &init);

  init.Pin = ADS1220_MISO_PIN;
  init.Mode = GPIO_MODE_INPUT;
  init.Pull = GPIO_PULLUP;
  GPIO_Init(ADS1220_PORT, &init);

  init.Pin = ADS1220_DRDY_PIN;
  GPIO_Init(ADS1220_DRDY_PORT, &init);
}

static void AD7793_SPI_Init(void)
{
  GPIO_InitTypeDef init;

  spi_channel_init(AD7793_CHANNEL, 1);

  SPI_CLK_IDLE(AD7793_CHANNEL);
  init.Pin = AD7793_CS_PIN | AD7793_DIN_PIN | AD7793_SCK_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(AD7793_PORT, &init);

  init.Pin = AD7793_DOUT_PIN;
  init.Mode = GPIO_MODE_INPUT;
  init.Pull = GPIO_PULLUP;
  GPIO_Init(AD7793_PORT, &init);
}

void SystemInit(void)
{
  capacity_measurement_done = 0;

  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOAEN;

  GPIOInit();

  if (InitRCC(&rcc_config))
  {
    LED_ON;
    while (1)
      ;
  }

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  USB_Init();
  TIM2Init();
  SPI2Init();
  ADS1220_SPI_Init();
  AD7793_SPI_Init();
}

void _init(void)
{
}

void discharge_off(void)
{
  GPIO_InitTypeDef init;

  init.Pin = R_DISCHARGE_PIN;
  init.Mode = GPIO_MODE_ANALOG;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(R_PORT, &init);
}

void discharge_on(void)
{
  GPIO_InitTypeDef init;

  init.Pin = R_DISCHARGE_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(R_PORT, &init);
}

void charge_off(void)
{
  GPIO_InitTypeDef init;

  init.Pin = R_CHARGE1_PIN | R_CHARGE2_PIN;
  init.Mode = GPIO_MODE_ANALOG;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(R_PORT, &init);
}

void charge_on(int channel)
{
  GPIO_InitTypeDef init;

  init.Pin = channel ? R_CHARGE2_PIN : R_CHARGE1_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(R_PORT, &init);
}
unsigned int get_capacity_measurement_start_time(void)
{
  return TIM2->CCR4;
}

unsigned int get_capacity_measurement_end_time(void)
{
  return TIM2->CCR1;
}

void capacity_measurement_start(int channel)
{
  discharge_off();
  TIM2->CNT = 0;
  TIM2->DIER = TIM_DIER_CC1IE;
  TIM2->SR = 0;
  //start TIM2
  TIM2->CR1 = TIM_CR1_CEN;
  charge_on(channel);
}

void TIM2_IRQHandler(void)
{
  capacity_measurement_done = 1;
  //stop timer
  TIM2->CR1 = 0;
  //disable TIM2 interrupts
  TIM2->DIER = 0;
  //clear interrupt flags
  TIM2->SR = 0;
}

void SH1107_Write(int num_bytes, unsigned char dc, const unsigned char *buffer)
{
  if (dc)
    LCD_DC_SET;
  else
    LCD_DC_CLR;
  // spi fifo size is 16
  int l = num_bytes > 16 ? 16 : num_bytes;
  num_bytes -= l;
  while (l--)
    SPI_Send8(LCD_SPI_INSTANCE, *buffer++, SPI_TIMEOUT);
  LCD_SPI_INSTANCE->CR1 |= SPI_CR1_CSTART;
  while (num_bytes--)
    SPI_Send8(LCD_SPI_INSTANCE, *buffer++, SPI_TIMEOUT);
  SPI_WaitSend(LCD_SPI_INSTANCE, SPI_TIMEOUT);
}

void spi_delay(int channel)
{
  delay(1);
}

void SPI_MOSI_SET(int channel)
{
  if (channel == ADS1220_CHANNEL)
    ADS1220_PORT->BSRR = ADS1220_MOSI_PIN;
  else
    AD7793_PORT->BSRR = AD7793_DIN_PIN;
}

void SPI_MOSI_CLR(int channel)
{
  if (channel == ADS1220_CHANNEL)
    ADS1220_PORT->BSRR = ADS1220_MOSI_PIN << 16;
  else
    AD7793_PORT->BSRR = AD7793_DIN_PIN << 16;
}

int SPI_CHECK_MISO(int channel)
{
  if (channel == ADS1220_CHANNEL)
    return ADS1220_PORT->IDR & ADS1220_MISO_PIN;
  return AD7793_PORT->IDR & AD7793_DOUT_PIN;
}

void SPI_CLK_IDLE(int channel)
{
  if (channel == ADS1220_CHANNEL)
    ADS1220_PORT->BSRR = ADS1220_SCK_PIN;
  else
    AD7793_PORT->BSRR = AD7793_SCK_PIN;
}

void SPI_CLK_ACTIVE(int channel)
{
  if (channel == ADS1220_CHANNEL)
    ADS1220_PORT->BSRR = ADS1220_SCK_PIN << 16;
  else
    AD7793_PORT->BSRR = AD7793_SCK_PIN << 16;
}

void SPI_CS_SET(int channel)
{
  if (channel == ADS1220_CHANNEL)
    ADS1220_PORT->BSRR = ADS1220_CS_PIN;
  else
    AD7793_PORT->BSRR = AD7793_CS_PIN;
}

void SPI_CS_CLR(int channel)
{
  if (channel == ADS1220_CHANNEL)
    ADS1220_PORT->BSRR = ADS1220_CS_PIN << 16;
  else
    AD7793_PORT->BSRR = AD7793_CS_PIN << 16;
}

static void hal_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength)
{
  unsigned int length = wlength + rlength;
  unsigned char wbuffer[length];
  unsigned char rbuffer[length];
  if (wlength > 0)
    memcpy(wbuffer, wdata, wlength);
  memset(wbuffer + wlength, 0, rlength);
  spi_transfer(channel, wbuffer, rbuffer, length, 1);
  if (rlength > 0)
    memcpy(rdata, rbuffer + wlength, rlength);
}

void ad7793_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength)
{
  hal_spi_transfer(AD7793_CHANNEL, wdata, wlength, rdata, rlength);
}

void ads1220_spi_transfer(int channel, const unsigned char *wdata, unsigned int wlength, unsigned char *rdata,
                          unsigned int rlength)
{
  hal_spi_transfer(ADS1220_CHANNEL, wdata, wlength, rdata, rlength);
}
