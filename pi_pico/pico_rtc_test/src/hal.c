#include "board.h"
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <rtc_ds1307.h>
#include <rtc_ds3231.h>
#include <rtc_pcf8563.h>

volatile char uart_buffer[MAX_COMMAND_LINE_LENGTH];
volatile char *uart_buffer_p;

static void I2CInit(void)
{
  i2c_init(I2C_INST, I2C_SPEED);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA_PIN);
  gpio_pull_up(I2C_SCL_PIN);
}

void SystemInit(void)
{
  I2CInit();
}

int i2c_ds1307_write(const unsigned char *data, int data_length)
{
  const int rc = i2c_write_timeout_us(I2C_INST, DS1307_I2C_ADDRESS >> 1, data, data_length, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int i2c_ds1307_transfer(const unsigned char *wdata, int wdata_length, unsigned char *rdata, int rdata_length)
{
  int rc = i2c_write_timeout_us(I2C_INST, DS1307_I2C_ADDRESS >> 1, wdata, wdata_length, true, I2C_TIMEOUT);
  if (rc < 0)
    return -rc;
  rc = i2c_read_timeout_us(I2C_INST, DS1307_I2C_ADDRESS >> 1, rdata, rdata_length, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int i2c_ds3231_write(const unsigned char *data, int data_length)
{
  const int rc = i2c_write_timeout_us(I2C_INST, DS3231_I2C_ADDRESS >> 1, data, data_length, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int i2c_ds3231_transfer(const unsigned char *wdata, int wdata_length, unsigned char *rdata, int rdata_length)
{
  int rc = i2c_write_timeout_us(I2C_INST, DS3231_I2C_ADDRESS >> 1, wdata, wdata_length, true, I2C_TIMEOUT);
  if (rc < 0)
    return -rc;
  rc = i2c_read_timeout_us(I2C_INST, DS3231_I2C_ADDRESS >> 1, rdata, rdata_length, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int i2c_pcf8563_write(const unsigned char *data, int data_length)
{
  const int rc = i2c_write_timeout_us(I2C_INST, PCF8563_I2C_ADDRESS >> 1, data, data_length, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int i2c_pcf8563_transfer(const unsigned char *wdata, int wdata_length, unsigned char *rdata, int rdata_length)
{
  int rc = i2c_write_timeout_us(I2C_INST, PCF8563_I2C_ADDRESS >> 1, wdata, wdata_length, true, I2C_TIMEOUT);
  if (rc < 0)
    return -rc;
  rc = i2c_read_timeout_us(I2C_INST, PCF8563_I2C_ADDRESS >> 1, rdata, rdata_length, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}
