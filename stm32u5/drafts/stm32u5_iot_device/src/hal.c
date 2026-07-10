#include "board.h"
#include <rcc.h>
#include <rtc.h>
#include <spi_soft.h>
#include <spi.h>
#include <i2c.h>
#include <nvic.h>
#include <stdio.h>
#include <cc1101.h>
#include <scd4x.h>
#include <usb.h>
#include <adc.h>

const RCCConfig rcc_config =
{
  .hse_frequency = 25000000,
  .hsebypass = 0,
  .pll = {
    {.m = 5, .p_frequency = 0, .q_frequency = 48000000, .r_frequency = 30000000},
    {.m = 1, .p_frequency = 0, .q_frequency = 0, .r_frequency = 0},
    {.m = 1, .p_frequency = 0, .q_frequency = 0, .r_frequency = 0},
  },
  .hpre = 1,
  .ppre1 = 1,
  .ppre2 = 1,
  .ppre3 = 1,
  .dpre = 1,
  .main_clock_source = RCC_CFGR1_SW_0 | RCC_CFGR1_SW_1, // PLL1P
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

#ifndef SPI_SOFT
static const SPI_InitStruct spi_init = {
  .fifo_threshold = 0,
  .io_swap = 0,
  .alternate_function_gpio_control = 0,
  .comm_mode = SPI_COMM_MODE_FULLDUPLEX,
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
  .ss_output_enable = 1,
  .ti_mode = 0,
  .tx_dma_enable = 0,
  .rx_dma_enable = 0,
  .software_slave_management = 0,
  .internal_slave_select = 1,
  .baud_rate = 100000,
  .spi_clock = 48000000
};
#endif

static void GPIOInit(void)
{
  GPIO_InitTypeDef init;

  RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOCEN;

  LED_OFF;
  init.Pin = GPIO_Pin_13;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(GPIOC, &init);
}

#ifndef SPI_SOFT
static void SPIInit(void)
{
  GPIO_InitTypeDef init;

  SPI_CLOCK_SELECT;

  SPI_PORT_ENABLE;
  SPI_ENABLE;

  init.Pin = SPI_SCK_PIN;
  init.Mode = GPIO_MODE_AF_PP;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_NOPULL;
  init.Alternate = SPI_AF;
  GPIO_Init(SPI_SCK_PORT, &init);

  init.Pin = SPI_MOSI_PIN;
  GPIO_Init(SPI_MOSI_PORT, &init);

  init.Pin = CC1101_CS_PIN;
  GPIO_Init(CC1101_CS_PORT, &init);

  init.Pin = SPI_MISO_PIN;
  init.Pull = GPIO_PULLUP; // should be pulldown
  GPIO_Init(SPI_MISO_PORT, &init);

  SPI_Init(SPI_INST, &spi_init);
  SPI_Enable(SPI_INST);
}
#else
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
  init.Pull = GPIO_PULLUP; // should be pulldown
  GPIO_Init(SPI_MISO_PORT, &init);
}
#endif

static void I2CInit(void)
{
  GPIO_InitTypeDef init;

  I2C_CLOCK_SELECT;

  I2C_PORT_ENABLE;
  I2C_ENABLE;

  init.Pin = SCL_PIN;
  init.Mode = GPIO_MODE_AF_OD;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_PULLUP;
  init.Alternate = I2C_AF;
  GPIO_Init(SCL_PORT, &init);

  init.Pin = SDA_PIN;
  GPIO_Init(SDA_PORT, &init);

  I2C_Master_Init(I2C_INST, I2C_TIMINGS); // calculated by stm32cubemx
  I2C_Enable(I2C_INST);
}

void CC1101Init(void)
{
  GPIO_InitTypeDef init;

  CC1101_PORT_ENABLE;

  init.Pin = CC1101_GD0_PIN;
  init.Mode = GPIO_MODE_INPUT;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Pull = GPIO_PULLDOWN;
  GPIO_Init(CC1101_GD0_PORT, &init);

  init.Pin = CC1101_GD2_PIN;
  init.Pull = GPIO_PULLDOWN; // should be pullup
  GPIO_Init(CC1101_GD2_PORT, &init);

#ifdef SPI_SOFT
  cc1101_CSN_SET(0);
  init.Pin = CC1101_CS_PIN;
  init.Mode = GPIO_MODE_OUTPUT_PP;
  init.Pull = GPIO_NOPULL;
  GPIO_Init(CC1101_CS_PORT, &init);
#endif
}

void USB_BaseInit(void)
{
  GPIO_InitTypeDef init;

  RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOAEN;

  // Configure DM DP Pins
  init.Pin = GPIO_Pin_11 | GPIO_Pin_12;
  init.Mode = GPIO_MODE_AF_PP;
  init.Pull = GPIO_NOPULL;
  init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  init.Alternate = GPIO_AF10_OTG_FS;
  GPIO_Init(GPIOA, &init);

  RCC->CCIPR1 |= RCC_CCIPR1_ICLKSEL_1; // pll1_q as usb clock
}

void USB_Init(void)
{
  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;
  /* Enable the main PLL */
  RCC->CR |= RCC_CR_PLL1ON;
  /* Wait till the main PLL is ready */
  while((RCC->CR & RCC_CR_PLL1RDY) == 0)
    ;

  RCC->CFGR1 |= RCC_CFGR1_SW; // PLL1P as clock source
  /* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR1 & RCC_CFGR1_SWS) != RCC_CFGR1_SWS)
    ;

  PWR->SVMCR |= PWR_SVMCR_USV;

  // Enable USB FS Clock
  RCC->AHB2ENR1 |= RCC_AHB2ENR1_OTGEN;
  NVIC_Init(OTG_FS_IRQn, 7, 0, ENABLE);
}

void USB_Deinit(void)
{
  NVIC_Init(OTG_FS_IRQn, 7, 0, DISABLE);
  RCC->AHB2ENR1 &= ~RCC_AHB2ENR1_OTGEN;
  PWR->SVMCR &= ~PWR_SVMCR_USV;

  RCC->CFGR1 &= ~RCC_CFGR1_SW; // msis as clock source
  /* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR1 & RCC_CFGR1_SWS) != 0)
    ;

  RCC->CR &= ~RCC_CR_PLL1ON;
  RCC->CR &= ~RCC_CR_HSEON;
}

static void ADCInit(void)
{
  GPIO_InitTypeDef init;

  // Configure VBUS pin
  init.Pin = GPIO_Pin_7;
  init.Mode = GPIO_MODE_ANALOG;
  init.Pull = GPIO_NOPULL;
  init.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_Init(GPIOA, &init);

  RCC->CCIPR3 |= RCC_CCIPR3_ADCDACSEL_2 | RCC_CCIPR3_ADCDACSEL_0; //MSIK as ADC clock
  RCC->AHB3ENR |= RCC_AHB3ENR_ADC4EN;
  ADC_Init(ADC4, ADC4_COMMON, 1, 1);
  ADC_Enable(ADC4);
}

void SystemInit(void)
{
  GPIOInit();

  if (InitRCC(&rcc_config))
  {
    LED_ON;
    while (1)
      __WFI();
  }

  USB_BaseInit();
  USB_Deinit();

  RTCInit(RCC_BDCR_LSEDRV_1 | RCC_BDCR_LSEDRV_0);
  SPIInit();
  I2CInit();
  CC1101Init();
  ADCInit();
}

void _init(void)
{

}

void OTG_FS_IRQHandler(void)
{
  USBInterruptHandler();
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

#ifdef SPI_SOFT
  spi_transfer(0, txdata, rxdata, (int)size, 1);
#else
  rc = SPI_TransmitReceive(SPI_INST, txdata, rxdata, size, SPI_TIMEOUT);
#endif

  cc1101_CSN_SET(device_num);

#ifdef SPI_SOFT
  return 0;
#else
  return rc;
#endif
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

#ifdef SPI_SOFT
  spi_transfer(0, &data, status, 1, 1);
#else
  rc = SPI_TransmitReceive(SPI_INST, &data, status, 1, SPI_TIMEOUT);
#endif

  cc1101_CSN_SET(device_num);

#ifdef SPI_SOFT
  return 0;
#else
  return rc;
#endif
}

int scd4x_write(const unsigned char *data, unsigned int len, bool no_ack_expected)
{
  int rc = I2C_Write(I2C_INST, SCD4X_SENSOR_ADDR << 1, data, len, I2C_TIMEOUT, true);
  if (rc == 5)
    return I2C_Write(I2C_INST, SCD4X_SENSOR_ADDR << 1, data, len, I2C_TIMEOUT, true);
  return rc;
}

int scd4x_read(unsigned char *data, unsigned int len)
{
  return I2C_Read(I2C_INST, data, len, I2C_TIMEOUT);
}

unsigned int adc_getvbus(void)
{
  return ADC_GetValue(ADC4, 20, ADC_SampleTime_160_5Cycles);
}

unsigned int adc_getvbat(void)
{
  return ADC_GetValue(ADC4, 14, ADC_SampleTime_160_5Cycles);
}
