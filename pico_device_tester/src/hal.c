#include "board.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include <spi_3wire_soft.h>
#include <max7219.h>
#include <spi_4bit_soft.h>
#include <swim.h>
#include <hardware/sync.h>
#include <stdio.h>

static unsigned int saved_ints;

void SystemInit(void)
{
  i2c_init(i2c_default, 100000);
  gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
  gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

  gpio_init(SWIM_IN_PIN);
  gpio_init(SWIM_OUT_PIN);
  SWIM_SET(SWIM_SET_VALUE);
  gpio_set_dir(SWIM_IN_PIN, false);
  gpio_set_dir(SWIM_OUT_PIN, true);
  gpio_set_drive_strength(SWIM_OUT_PIN, GPIO_DRIVE_STRENGTH_12MA);
  gpio_pull_up(SWIM_IN_PIN);

  gpio_init(SPI3_DIO_PIN);
  gpio_init(SPI3_CS_PIN);
  gpio_init(SPI3_CLK_PIN);
  gpio_put(SPI3_CS_PIN, true);
  gpio_put(SPI3_CLK_PIN, true);
  gpio_put(SPI3_DIO_PIN, false);
  gpio_set_dir(SPI3_CLK_PIN, true);
  gpio_set_dir(SPI3_CS_PIN, true);
  SPI3_DIO_SET;
  gpio_pull_up(SPI3_DIO_PIN);

  gpio_init(SPI4_DIO0_PIN);
  gpio_init(SPI4_DIO1_PIN);
  gpio_init(SPI4_DIO2_PIN);
  gpio_init(SPI4_DIO3_PIN);
  gpio_init(SPI4_CLK_PIN);
  gpio_init(SPI4_CS_PIN);
  gpio_put(SPI4_CS_PIN, true);
  gpio_put(SPI4_CLK_PIN, false);
  gpio_set_dir(SPI4_DIO0_PIN, true);
  gpio_set_dir(SPI4_DIO1_PIN, true);
  gpio_set_dir(SPI4_DIO2_PIN, true);
  gpio_set_dir(SPI4_DIO3_PIN, true);
  gpio_set_dir(SPI4_CLK_PIN, true);
  gpio_set_dir(SPI4_CS_PIN, true);
}

void swim_enter_critical_section(void)
{
  saved_ints = save_and_disable_interrupts();
}

void swim_leave_critical_section(void)
{
  restore_interrupts(saved_ints);
}

void spi3_delay(void)
{
  sleep_us(1);
}

void max7219_send(unsigned short **data)
{
  spi_4bit_send16(data, MAX7219_NUM_PARALLEL, MAX7219_NUM_SERIES);
}

void spi_4bit_delay(void)
{
  sleep_us(10);
}
