#include "board.h"
#include <rcc.h>
#include <rtc.h>
#include <spi_soft.h>
#include <i2c_soft.h>
#include <usart.h>
#include <nvic.h>
#include <stdio.h>
#include <cc1101.h>
#include <scd4x.h>

const RCCConfig rcc_config =
{
  .hse_frequency = 8000000,
  .hsebypass = 1,
  .pll = {
    {.m = 1, .p_frequency = 0, .q_frequency = 0, .r_frequency = 0},
    {.m = 1, .p_frequency = 0, .q_frequency = 0, .r_frequency = 0},
    {.m = 1, .p_frequency = 0, .q_frequency = 0, .r_frequency = 0},
  },
  .hpre = 1,
  .ppre1 = 1,
  .ppre2 = 1,
  .ppre3 = 1,
  .dpre = 1,
  .main_clock_source = 0, // MSIS
  .ahb1dis = false,
  .ahb2dis1 = false,
  .ahb2dis2 = false,
  .ahb3dis = false,
  .apb1dis = false,
  .apb2dis = false,
  .apb3dis = false,
  .boosten = false,
  .flash_pdreq1 = false,
  .flash_pdreq2 = false,
  .flash_sleep_pd = false,
  .usbhsboosten = false,
  .usbhspwren = false,
  .vdd11usbdis = false
};

static char usart_buffer[USART_BUFFER_SIZE];
static char *usart_buffer_write_p, *usart_buffer_read_p;

void LPUART1_IRQHandler(void)
{
  while (LPUART1->ISR & USART_ISR_RXNE_RXFNE)
  {
    unsigned char c = LPUART1->RDR;
    *usart_buffer_write_p++ = c;
    if (usart_buffer_write_p == usart_buffer + USART_BUFFER_SIZE)
      usart_buffer_write_p = usart_buffer;
  }
}

static void GPIOInit(void)
{
  GPIO_InitTypeDef init;

  RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOCEN | RCC_AHB2ENR1_GPIOBEN | RCC_AHB2ENR1_GPIOGEN;

  PWR->SVMCR |= PWR_SVMCR_IO2SV; // enable portg[15:2]

  LED_GREEN_OFF;
  init.Pin = GPIO_Pin_7;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(GPIOC, &init);
  LED_BLUE_OFF;
  GPIO_Init(GPIOB, &init);
  LED_RED_OFF;
  init.Pin = GPIO_Pin_2;
  GPIO_Init(GPIOG, &init);
}

static void SPIInit(void)
{
  GPIO_InitTypeDef init;

  spi_channel_init(0, 0);

  SPI_PORT_ENABLE;

  init.Pin = SPI_SCK_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(SPI_SCK_PORT, &init);

  init.Pin = SPI_MOSI_PIN;
  GPIO_Init(SPI_MOSI_PORT, &init);

  init.Pin = SPI_MISO_PIN;
  init.Mode = GPIO_MODE_INPUT;
  init.Pull = GPIO_PULLDOWN;
  GPIO_Init(SPI_MISO_PORT, &init);
}

static void I2CInit(void)
{
  GPIO_InitTypeDef init;

  I2C_PORT_ENABLE;

  i2c_soft_init(0);

  init.Pin = SCL_PIN;
  init.Mode = GPIO_MODE_OUTPUT_OD;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_PULLUP;
  GPIO_Init(SCL_PORT, &init);

  init.Pin = SDA_PIN;
  GPIO_Init(SDA_PORT, &init);
}

static void LPUARTInit(void)
{
  GPIO_InitTypeDef init;

  RCC->CCIPR3 = 4; // MSIK as clock

  RCC->APB3ENR |= RCC_APB3ENR_LPUART1EN;

  // Configure RX TX Pins
  init.Pin = GPIO_Pin_7 | GPIO_Pin_8;
  init.Mode = GPIO_MODE_AF_PP;
  init.Pull = GPIO_NOPULL;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Alternate = GPIO_AF8_LPUART1;
  GPIO_Init(GPIOG, &init);

  LPUART1->BRR = 0x1A0AA; // 9600 baud, 4MHz clock
  LPUART1->CR1 = USART_CR1_RE | USART_CR1_UESM | USART_CR1_TE | USART_CR1_RXNEIE_RXFNEIE | USART_CR1_FIFOEN;

  LPUART1->ICR = 0x2025F;

  NVIC_Init(LPUART1_IRQn, 1, 3, ENABLE);
}

void CC1101Init(void)
{
  GPIO_InitTypeDef init;

  init.Pin = CC1101_GD0_PIN;
  init.Mode = GPIO_MODE_INPUT;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_PULLDOWN;
  GPIO_Init(CC1101_GD0_PORT, &init);

  init.Pin = CC1101_GD2_PIN;
  init.Pull = GPIO_PULLDOWN; // should be pullup
  GPIO_Init(CC1101_GD2_PORT, &init);

  cc1101_CSN_SET(0);
  init.Pin = CC1101_CS_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(CC1101_CS_PORT, &init);
}

/*static void InitMSIK(void)
{
  RCC->CR &= ~RCC_CR_MSIKON;
  RCC->ICSCR1 &= ~RCC_ICSCR1_MSIKRANGE; // Clear current range bits

  unsigned int temp = RCC->CR;
  // Set MSIPLLSEL to 0 (Select MSIK as the primary target for LSE)
  temp &= ~RCC_CR_MSIPLLSEL;
  // Turn on the hardware auto-calibration PLL-mode
  RCC->CR = temp | RCC_CR_MSIPLLEN;

  RCC->CR |= RCC_CR_MSIKON;

  // Wait for the MSIS clock branch to become stable and ready
  while (!(RCC->CR & RCC_CR_MSIKRDY))
    ;
  while (!(RCC->CR & RCC_CR_MSIPLLEN))
    ;
}*/

/*static void USBInit(void)
{
  GPIO_InitTypeDef init;

  RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOAEN;

  // Configure DM DP Pins
  init.Pin = (GPIO_Pin_11 | GPIO_Pin_12);
  init.Mode = GPIO_MODE_AF_PP;
  init.Pull = GPIO_NOPULL;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Alternate = GPIO_AF10_OTG_FS;
  GPIO_Init(GPIOA, &init);

  PWR->SVMCR |= PWR_SVMCR_USV;

  RCC->CCIPR1 |= RCC_CCIPR1_ICLKSEL_1; // pll1_q as usb clock
  // Enable USB FS Clock
  RCC->AHB2ENR1 |= RCC_AHB2ENR1_OTGEN;
}*/

void SystemInit(void)
{
  if (InitRCC(&rcc_config))
  {
    while (1)
      ;
  }

  //InitMSIK();

  GPIOInit();
  RTCInit(RCC_BDCR_LSEDRV_1 | RCC_BDCR_LSEDRV_0);
  SPIInit();
  I2CInit();
  LPUARTInit();
  //USBInit();
  CC1101Init();
}

void _init(void)
{

}

void puts_(const char *s)
{
  for (;;)
  {
    char c = *s++;
    if (!c)
      return;
    usart_send_char(LPUART1, c);
  }
}

int getch_(void)
{
  if (usart_buffer_write_p != usart_buffer_read_p)
  {
    char c = *usart_buffer_read_p++;
    if (usart_buffer_read_p == usart_buffer + USART_BUFFER_SIZE)
      usart_buffer_read_p = usart_buffer;
    return c;
  }
  return EOF;
}

void usart_start(void)
{
  usart_buffer_write_p = usart_buffer_read_p = usart_buffer;

  LPUART1->CR1 |= USART_CR1_UE;
}

int cc1101_RW(unsigned int device_num, unsigned char *txdata, unsigned char *rxdata, unsigned int size)
{
  int rc;

  if (size < 2)
    return 1;

  rc = CC1101_TIMEOUT;
  while (--rc && cc1101_GD2) // waiting for chip ready
    ;
  if (!rc)
    return 2; // timeout

  if (size > 2)
    txdata[0] |= CC1101_BURST;

  cc1101_CSN_CLR(device_num);

  spi_transfer(0, txdata, rxdata, (int)size, 1);

  cc1101_CSN_SET(device_num);

  return 0;
}

int cc1101_strobe(unsigned int device_num, unsigned char data, unsigned char *status)
{
  int rc;

  rc = CC1101_TIMEOUT;
  while (--rc && cc1101_GD2) // waiting for chip ready
    ;
  if (!rc)
    return 1; // timeout

  cc1101_CSN_CLR(device_num);

  spi_transfer(0, &data, status, 1, 1);

  cc1101_CSN_SET(device_num);

  return 0;
}

int scd4x_write(const unsigned char *data, unsigned int len, bool no_ack_expected)
{
  return i2c_soft_write(0, SCD4X_SENSOR_ADDR << 1, data, len, no_ack_expected, I2C_TIMEOUT);
}

int scd4x_read(unsigned char *data, unsigned int len)
{
  return i2c_soft_read(0, SCD4X_SENSOR_ADDR << 1, data, len, I2C_TIMEOUT);
}

int scd4x_command(const unsigned char *wdata, unsigned int wlen, unsigned char *rdata, unsigned int rlen)
{
  return i2c_soft_command(0, SCD4X_SENSOR_ADDR << 1, nullptr, 0, wdata, wlen, rdata, rlen, I2C_TIMEOUT);
}
