#include "board.h"
#include <i2c_soft.h>
#include <ina226.h>
#include <mcp3421.h>
#include <ads1115.h>
#include <generic_dds.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "led_strip.h"
#include "dev_si5351.h"

static const int sda_pins[MAX_DEVICES] = {SDA_PINS};
static const int scl_pins[MAX_DEVICES] = {SCL_PINS};

static unsigned int s_led_state;

#ifdef LED_STRIP
static led_strip_handle_t led_strip;

void configure_led(void)
{
  /* LED strip initialization with the GPIO and pixels number*/
  led_strip_config_t strip_config = {
      .strip_gpio_num = PIN_LED,
      .max_leds = 1, // at least one LED on board
  };
  led_strip_rmt_config_t rmt_config = {
      .resolution_hz = 10 * 1000 * 1000, // 10MHz
  };
  ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
  /* Set all LED off to clear all pixels */
  led_strip_clear(led_strip);

  s_led_state = 0;
}

static void set_led(uint32_t red, uint32_t green, uint32_t blue)
{
  /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
  led_strip_set_pixel(led_strip, 0, red, green, blue);
  /* Refresh the strip to send data */
  led_strip_refresh(led_strip);
}

void blink_led(void)
{
  s_led_state = s_led_state ? 0 : 8;
  set_led(0, s_led_state, 0);
}
#else
void configure_led(void)
{
  gpio_reset_pin(PIN_LED);
  /* Set the GPIO as a push/pull output */
  gpio_set_direction(PIN_LED, GPIO_MODE_OUTPUT);

  s_led_state = 0;
}

void blink_led(void)
{
  s_led_state = !s_led_state;
  gpio_set_level(PIN_LED, s_led_state);
}
#endif

void configure_uart(void)
{
  uart_config_t uart_config = {
      .baud_rate = UART_BAUD_RATE,
      .data_bits = UART_DATA_8_BITS,
      .parity    = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };

  uart_driver_install(UART_PORT, UART_BUFFER_SIZE, UART_BUFFER_SIZE, 0, NULL, ESP_INTR_FLAG_IRAM);
  uart_param_config(UART_PORT, &uart_config);
  uart_set_pin(UART_PORT, PIN_UART_TXD, PIN_UART_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

void configure_i2c(void)
{
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    int pin = sda_pins[i];
    gpio_set_level(pin, 1);
    gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
    pin = scl_pins[i];
    gpio_set_level(pin, 1);
    gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
  }
}

void SCL_HIGH(int channel)
{
  gpio_set_level(scl_pins[channel], 1);
}

void SCL_LOW(int channel)
{
  gpio_set_level(scl_pins[channel], 0);
}

void SDA_HIGH(int channel)
{
  gpio_set_level(sda_pins[channel], 1);
}

void SDA_LOW(int channel)
{
  gpio_set_level(sda_pins[channel], 0);
}

int SDA_IN(int channel)
{
  return gpio_get_level(sda_pins[channel]);
}

int SCL_IN(int channel)
{
  return gpio_get_level(scl_pins[channel]);
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

void delayus(unsigned int us)
{
  esp_rom_delay_us(us);
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
