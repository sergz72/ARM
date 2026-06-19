#include "board.h"
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <bh1750.h>
#include <tsl2591.h>
#include <veml7700.h>

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

void delayms(unsigned int ms)
{
  sleep_ms(ms);
}

void delay(unsigned int us)
{
  sleep_us(us);
}

void SystemInit(void)
{
  I2CInit();
}

int veml7700_read(unsigned char reg, unsigned short *data)
{
  int rc = i2c_write_timeout_us(I2C_INST, VEML7700_I2C_ADDRESS, &reg, 1, true, I2C_TIMEOUT);
  if (rc < 0)
    return -rc;
  rc = i2c_read_timeout_us(I2C_INST, VEML7700_I2C_ADDRESS, (unsigned char*)data, 2, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int veml7700_write(unsigned char reg, unsigned short value)
{
  unsigned char data[3] = {reg, (unsigned char)value, (unsigned char)(value >> 8)};
  const int rc = i2c_write_timeout_us(I2C_INST, VEML7700_I2C_ADDRESS, data, 3, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int tsl2591_read8(unsigned char reg, unsigned char *data)
{
  int rc = i2c_write_timeout_us(I2C_INST, TSL2591_I2C_ADDRESS, &reg, 1, true, I2C_TIMEOUT);
  if (rc < 0)
    return -rc;
  rc = i2c_read_timeout_us(I2C_INST, TSL2591_I2C_ADDRESS, data, 1, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int tsl2591_read16(unsigned char reg, unsigned short *data)
{
  int rc = i2c_write_timeout_us(I2C_INST, TSL2591_I2C_ADDRESS, &reg, 1, true, I2C_TIMEOUT);
  if (rc < 0)
    return -rc;
  rc = i2c_read_timeout_us(I2C_INST, TSL2591_I2C_ADDRESS, (unsigned char*)data, 2, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int tsl2591_write(unsigned char reg, unsigned char value)
{
  unsigned char data[2] = {reg, value};
  const int rc = i2c_write_timeout_us(I2C_INST, TSL2591_I2C_ADDRESS, data, 2, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int tsl2591_command(unsigned char command)
{
  const int rc = i2c_write_timeout_us(I2C_INST, TSL2591_I2C_ADDRESS, &command, 1, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int bh1750_write(unsigned char command)
{
  const int rc = i2c_write_timeout_us(I2C_INST, BH1750_ADDR, &command, 1, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}

int bh1750_read(unsigned char *result)
{
  const int rc = i2c_read_timeout_us(I2C_INST, BH1750_ADDR, result, 2, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}
