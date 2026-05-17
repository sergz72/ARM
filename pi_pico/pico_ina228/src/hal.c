#include "board.h"
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <ina226.h>
#include <ina228.h>

static void I2CInit(void)
{
  i2c_init(I2C_INST, 100000);
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

int ina226ReadRegister(int channel, unsigned char address, unsigned char reg, unsigned short *data)
{
  unsigned char d[2];
  int rc = i2c_write_timeout_us(I2C_INST, address, &reg, 1, true, I2C_TIMEOUT);
  if (rc < 0)
    return -rc;
  rc = i2c_read_timeout_us(I2C_INST, address, d, 2, false, I2C_TIMEOUT);
  if (rc >= 0)
      *data = (d[0] << 8) | d[1];
  return rc >= 0 ? 0 : -rc;
}

int ina226WriteRegister(int channel, unsigned char address, unsigned char reg, unsigned short data)
{
  unsigned char d[3];
  d[0] = reg;
  d[1] = data >> 8;
  d[2] = data;
  const int rc = i2c_write_timeout_us(I2C_INST, address, d, 3, false, I2C_TIMEOUT);
  return rc >= 0 ? 0 : -rc;
}
