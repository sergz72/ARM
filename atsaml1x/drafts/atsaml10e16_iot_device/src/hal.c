#include "board.h"
#include <cc1101.h>
#include <scd4x.h>
#include <rtc_ds1307.h>
#include <rtc_ds3231.h>
#include <rtc_pcf8563.h>
#include <usart.h>
#include <i2c.h>
#include <spi.h>

/*
 * PA00(1) = LED
 */
static void ports_init(void)
{
  LED_TIMER_OFF;
  PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA00;
}

static void clock_init(void)
{
  // EIC
  GCLK_REGS->GCLK_PCHCTRL[3] = GCLK_PCHCTRL_CHEN(1);
  // SERCOM0_CORE - USART
  GCLK_REGS->GCLK_PCHCTRL[11] = GCLK_PCHCTRL_CHEN(1);
  // SERCOM1_CORE - I2C
  GCLK_REGS->GCLK_PCHCTRL[12] = GCLK_PCHCTRL_CHEN(1);
  // SERCOM2_CORE - SPI
  GCLK_REGS->GCLK_PCHCTRL[13] = GCLK_PCHCTRL_CHEN(1);
  // ADC
  GCLK_REGS->GCLK_PCHCTRL[16] = GCLK_PCHCTRL_CHEN(1);
}

void SysInit(void)
{
  clock_init();
  ports_init();
  usart_init();
  i2c_master_init();
}

void _init(void)
{
}

int cc1101_RW(unsigned int device_num, unsigned char *txdata, unsigned char *rxdata, unsigned int size)
{
  int rc;

  if (size < 2)
    return 1;

  cc1101_CSN_CLR(device_num);

  rc = CC1101_TIMEOUT;
  while (--rc && cc1101_GD2) // waiting for chip ready
    ;
  if (!rc)
  {
    cc1101_CSN_SET(device_num);
    return 2; // timeout
  }

  if (size > 2)
    txdata[0] |= CC1101_BURST;

  //rc = SPI_TransmitReceive(SPI_INST, txdata, rxdata, size, SPI_TIMEOUT);

  cc1101_CSN_SET(device_num);

  return 0;
}

int cc1101_strobe(unsigned int device_num, unsigned char data, unsigned char *status)
{
  int rc;

  cc1101_CSN_CLR(device_num);

  rc = CC1101_TIMEOUT;
  while (--rc && cc1101_GD2) // waiting for chip ready
    ;
  if (!rc)
  {
    cc1101_CSN_SET(device_num);
    return 1; // timeout
  }

  //rc = SPI_TransmitReceive(SPI_INST, &data, status, 1, SPI_TIMEOUT);

  cc1101_CSN_SET(device_num);

  return 0;
}

int scd4x_write(const unsigned char *data, unsigned int len, bool no_ack_expected)
{
  return i2c_write(SCD4X_SENSOR_ADDR << 1, data, len);
}

int scd4x_read(unsigned char *data, unsigned int len)
{
  return i2c_read(SCD4X_SENSOR_ADDR << 1, data, len);
}

int i2c_ds1307_write(const unsigned char *data, int data_length)
{
  return i2c_write(DS1307_I2C_ADDRESS, data, data_length);
}

int i2c_ds1307_transfer(const unsigned char *wdata, int wdata_length, unsigned char *rdata, int rdata_length)
{
  return i2c_transfer(DS1307_I2C_ADDRESS, wdata, wdata_length, rdata, rdata_length);
}

int i2c_ds3231_write(const unsigned char *data, int data_length)
{
  return i2c_write(DS3231_I2C_ADDRESS, data, data_length);
}

int i2c_ds3231_transfer(const unsigned char *wdata, int wdata_length, unsigned char *rdata, int rdata_length)
{
  return i2c_transfer(DS1307_I2C_ADDRESS, wdata, wdata_length, rdata, rdata_length);
}

int i2c_pcf8563_write(const unsigned char *data, int data_length)
{
  return i2c_write(PCF8563_I2C_ADDRESS, data, data_length);
}

int i2c_pcf8563_transfer(const unsigned char *wdata, int wdata_length, unsigned char *rdata, int rdata_length)
{
  return i2c_transfer(PCF8563_I2C_ADDRESS, wdata, wdata_length, rdata, rdata_length);
}
