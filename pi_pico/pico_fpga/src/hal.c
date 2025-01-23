#include "board.h"
#include <i2c_soft.h>
#include <ina226.h>
#include <mcp3421.h>
#include <ads1115.h>
#include <generic_dds.h>
#include <stdio.h>
#include <pico/stdio.h>
#include "dev_si5351.h"
#include <pico/time.h>

static bool led_state;

void configure_ports(void)
{
  //todo
  gpio_init(PIN_RESET);
  gpio_set_dir(PIN_RESET, GPIO_OUT);
  gpio_init(PIN_LED);
  gpio_set_dir(PIN_LED, GPIO_OUT);
  gpio_init(PIN_DEVICE_ID0);
  gpio_set_dir(PIN_DEVICE_ID0, GPIO_OUT);
  gpio_init(PIN_DEVICE_ID1);
  gpio_set_dir(PIN_DEVICE_ID1, GPIO_OUT);
  gpio_init(PIN_DEVICE_ID2);
  gpio_set_dir(PIN_DEVICE_ID2, GPIO_OUT);

  led_state = false;
}

void blink_led(void)
{
  led_state = !led_state;
  gpio_put(PIN_LED, led_state);
}

void configure_logger(void)
{
  //todo
}

void configure_spi(void)
{
  gpio_init(PIN_SCLK);
  gpio_put(PIN_SCLK, true);
  gpio_set_dir(PIN_SCLK, GPIO_OUT);
  gpio_init(PIN_SNCS);
  gpio_put(PIN_SNCS, true);
  gpio_set_dir(PIN_SNCS, GPIO_OUT);

  gpio_init(PIN_SDI0);
  gpio_set_dir(PIN_SDI0, GPIO_IN);
  gpio_init(PIN_SDI1);
  gpio_set_dir(PIN_SDI1, GPIO_IN);
  gpio_init(PIN_SDI2);
  gpio_set_dir(PIN_SDI2, GPIO_IN);

  gpio_init(PIN_SDO0);
  gpio_set_dir(PIN_SDO0, GPIO_OUT);
  gpio_init(PIN_SDO1);
  gpio_set_dir(PIN_SDO1, GPIO_OUT);
  gpio_init(PIN_SDO2);
  gpio_set_dir(PIN_SDO2, GPIO_OUT);
}

void configure_i2c(void)
{
  gpio_init(PIN_I2C_SDA_OE);
  gpio_init(PIN_I2C_SCL_OE);
  gpio_init(PIN_I2C_SDA_IN);
  gpio_init(PIN_I2C_SCL_IN);
  i2c_soft_init(0);
  gpio_set_dir(PIN_I2C_SDA_OE, GPIO_OUT);
  gpio_set_dir(PIN_I2C_SCL_OE, GPIO_OUT);
  gpio_set_dir(PIN_I2C_SDA_IN, GPIO_IN);
  gpio_set_dir(PIN_I2C_SCL_IN, GPIO_IN);
}

void change_channel(int channel)
{
  gpio_put(PIN_DEVICE_ID0, channel & 1);
  gpio_put(PIN_DEVICE_ID1, channel & 2);
  gpio_put(PIN_DEVICE_ID2, channel & 4);
}

void SPI_3BIT_DATA_SET(unsigned int data)
{
  gpio_put(PIN_SDO0, data & 0x20000000);
  gpio_put(PIN_SDO1, data & 0x40000000);
  gpio_put(PIN_SDO2, data & 0x80000000);
}

unsigned int spi3_data_get(void)
{
  unsigned int data = gpio_get(PIN_SDI0) ? 1 : 0;
  if (gpio_get(PIN_SDI1))
    data |= 2;
  if (gpio_get(PIN_SDI2))
    data |= 4;
  return data;
}

void SCL_HIGH(int channel)
{
  gpio_put(PIN_I2C_SCL_OE, true);
}

void SCL_LOW(int channel)
{
  gpio_put(PIN_I2C_SCL_OE, false);
}

void SDA_HIGH(int channel)
{
  gpio_put(PIN_I2C_SDA_OE, true);
}

void SDA_LOW(int channel)
{
  gpio_put(PIN_I2C_SDA_OE, false);
}

int SDA_IN(int channel)
{
  return gpio_get(PIN_I2C_SDA_IN);
}

int SCL_IN(int channel)
{
  return gpio_get(PIN_I2C_SCL_IN);
}

int I2C_SendAddress(int idx, int address)
{
  i2c_soft_start(idx);

  if (i2c_soft_tx(idx, address, I2C_TIMEOUT)) // no ack
  {
    i2c_soft_stop(idx);
    return 1;
  }

  return 0;
}

int I2CCheck(int idx, int device_id)
{
  int rc;

  rc = I2C_SendAddress(idx, device_id);
  if (!rc)
    i2c_soft_stop(idx);
  return rc == 0;
}

int inaReadRegister(int channel, unsigned char address, unsigned char reg, unsigned short *data)
{
  unsigned char d[2];
  int rc = i2c_soft_command(channel, address, &reg, 1, NULL, 0, d, 2, I2C_TIMEOUT);
  if (!rc)
    *data = (d[0] << 8) | d[1];
  return rc;
}

int ads1115ReadRegister(int channel, unsigned char address, unsigned char reg, unsigned short *data)
{
  return inaReadRegister(channel, address, reg, data);
}

int mcp3421Read(int channel, unsigned char address, unsigned char *data, unsigned int l)
{
  return i2c_soft_read(channel, address, data, l, I2C_TIMEOUT);
}

int inaWriteRegister(int channel, unsigned char address, unsigned char reg, unsigned short data)
{
  unsigned char d[2];
  d[0] = data >> 8;
  d[1] = data & 0xFF;
  return i2c_soft_command(channel, address, &reg, 1, d, 2, NULL, 0, I2C_TIMEOUT);
}

int ads1115WriteRegister(int channel, unsigned char address, unsigned char reg, unsigned short data)
{
  return inaWriteRegister(channel, address, reg, data);
}

int mcp3421Write(int channel, unsigned char address, unsigned char data)
{
  return i2c_soft_command(channel, address, NULL, 0, &data, 1, NULL, 0, I2C_TIMEOUT);
}

unsigned long long int time_us(void)
{
  return time_us_64();
}

void delay_us(unsigned int us)
{
  sleep_us(us);
}

int dds_command(unsigned char deviceId, unsigned char cmd, dds_cmd *data, int idx)
{
  dds_i2c_command c;
  switch (cmd)
  {
    case DDS_COMMAND_ENABLE_OUTPUT:
      c.c3.command = cmd;
      c.c3.channel = data->channel;
      c.c3.parameter = data->enable_command.enable;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 3,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    case DDS_COMMAND_SET_ATTENUATOR:
      c.c4.command = cmd;
      c.c4.channel = data->channel;
      c.c4.parameter = data->set_attenuator_command.attenuator_value;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 4,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    case DDS_COMMAND_SET_FREQUENCY:
      c.c8.command = cmd;
      c.c8.channel = data->channel;
      c.c8.freq = data->set_frequency_command.frequency;
      c.c8.div = data->set_frequency_command.divider;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 8,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    case DDS_COMMAND_SET_FREQUENCY_CODE:
      c.c8.command = cmd;
      c.c8.channel = data->channel;
      c.c8.freq = data->set_frequency_code_command.frequency_code;
      c.c8.div = data->set_frequency_code_command.divider;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 8,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    case DDS_COMMAND_SET_MODE:
      c.c3.command = cmd;
      c.c3.channel = data->channel;
      c.c3.parameter = data->set_mode_command.mode;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 3,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    default:
      return 1;
  }
}

int si5351_write_bulk(int channel, unsigned char addr, unsigned char bytes, unsigned char *data)
{
  return i2c_soft_command(channel, SI5351_DEVICE_ID, &addr, 1, data, bytes, NULL, 0, I2C_TIMEOUT);
}

int si5351_write(int channel, unsigned char addr, unsigned char data)
{
  return i2c_soft_command(channel, SI5351_DEVICE_ID, &addr, 1, &data, 1, NULL, 0, I2C_TIMEOUT);
}

int dds_get_config(dds_config *cfg, unsigned char deviceId, int idx)
{
  return i2c_soft_read(idx, deviceId, (unsigned char*)cfg, sizeof(dds_config), I2C_TIMEOUT);
}

int mcp9600Read16(int channel, unsigned char address, unsigned char reg, unsigned short *data)
{
  return inaReadRegister(channel, address, reg, data);
}

int mcp9600Read8(int channel, unsigned char address, unsigned char reg, unsigned char *data)
{
  return i2c_soft_command(channel, address, &reg, 1, NULL, 0, data, 1, I2C_TIMEOUT);
}

int mcp9600Write(int channel, unsigned char address, unsigned char reg, unsigned char data)
{
  return i2c_soft_command(channel, address, &reg, 1, &data, 1, NULL, 0, I2C_TIMEOUT);
}

void main_comm_port_write_bytes(const unsigned char *data, int len)
{
  while (len--)
    putchar_raw(*data++);
}

int main_comm_port_read_bytes(unsigned char *buffer, int buffer_size)
{
  int count = 0;
  while (1)
  {
    int ch = getchar_timeout_us(0);
    if (ch == PICO_ERROR_TIMEOUT)
      return count;
    *buffer++ = (unsigned char)ch;
    count++;
  }
}

void release_reset(void)
{
  gpio_put(PIN_RESET, true);
}
