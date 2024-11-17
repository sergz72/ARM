#include <memory.h>
#include "board.h"
#include "hardware/gpio.h"
#include <mcp9600.h>

#define MCP9600_DEVICE_ID 0xCE

#define KEYBOARD_PIN_MASK ((1 << (KEYBOARD_PIN_END - KEYBOARD_PIN_START + 1)) - 1)

static unsigned char i2c_buffer[1024];

void SystemInit(void)
{
  int i;

  i2c_init(I2C_INST, 100000);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA_PIN);
  gpio_pull_up(I2C_SCL_PIN);

  for (i = KEYBOARD_PIN_START; i <= KEYBOARD_PIN_END; i++)
  {
    gpio_init(i);
    gpio_set_dir(i, false);
    gpio_pull_up(i);
  }

  gpio_init(OVEN_CONTROL_PIN);
  gpio_set_dir(OVEN_CONTROL_PIN, true);
}

void delayms(unsigned int ms)
{
  sleep_ms(ms);
}

int SSD1306_I2C_Write(int num_bytes, unsigned char control_byte, unsigned char *buffer)
{
  i2c_buffer[0] = control_byte;
  memcpy(i2c_buffer + 1, buffer, num_bytes);
  int rc = i2c_write_timeout_us(I2C_INST, SSD1306_I2C_ADDRESS >> 1, i2c_buffer, num_bytes + 1, false, I2C_TIMEOUT);
  if (rc == PICO_ERROR_GENERIC || rc == PICO_ERROR_TIMEOUT)
    return 1;
  return 0;
}

int mcp9600Read16(int channel, unsigned char address, unsigned char reg, unsigned short *data)
{
  int rc = i2c_write_timeout_us(I2C_INST, address >> 1, &reg, 1, true, I2C_TIMEOUT);
  if (rc == PICO_ERROR_GENERIC || rc == PICO_ERROR_TIMEOUT)
    return rc;
  rc = i2c_read_timeout_us(I2C_INST, address >> 1, i2c_buffer, 2, false, I2C_TIMEOUT);
  if (rc == PICO_ERROR_GENERIC || rc == PICO_ERROR_TIMEOUT)
    return rc;
  *data = (i2c_buffer[0] << 8) | i2c_buffer[1];
  return 0;
}

int mcp9600Read8(int channel, unsigned char address, unsigned char reg, unsigned char *data)
{
  int rc = i2c_write_timeout_us(I2C_INST, address >> 1, &reg, 1, true, I2C_TIMEOUT);
  if (rc == PICO_ERROR_GENERIC || rc == PICO_ERROR_TIMEOUT)
    return rc;
  rc = i2c_read_timeout_us(I2C_INST, address >> 1, data, 1, false, I2C_TIMEOUT);
  if (rc == PICO_ERROR_GENERIC || rc == PICO_ERROR_TIMEOUT)
    return rc;
  return 0;
}

int mcp9600Write(int channel, unsigned char address, unsigned char reg, unsigned char data)
{
  i2c_buffer[0] = reg;
  i2c_buffer[1] = data;
  int rc = i2c_write_timeout_us(I2C_INST, address >> 1, i2c_buffer, 2, false, I2C_TIMEOUT);
  if (rc == PICO_ERROR_GENERIC || rc == PICO_ERROR_TIMEOUT)
    return rc;
  return 0;
}

unsigned int get_keyboard_status(void)
{
  unsigned int value = gpio_get_all();
  return (value >> KEYBOARD_PIN_START) & KEYBOARD_PIN_MASK;
}

void off(void)
{
  gpio_put(OVEN_CONTROL_PIN, false);
}

void on(void)
{
  gpio_put(OVEN_CONTROL_PIN, true);
}

void MCP9600Init(void)
{
  MCP9600DeviceConfig dcfg;
  MCP9600SensorConfig scfg;

  scfg.thermocouple_type = MCP9600_THERMOCOUPLE_TYPE_K;
  scfg.filter_coefficient = 0;
  dcfg.burst_mode_samples = MCP9600_BURST_MODE_1;
  dcfg.adc_resolution = MCP9600_ADC_RESOLUTION_18;
  dcfg.mode = MCP9600_MODE_NORMAL;
  dcfg.low_cold_juction_resolution = 0;

  mcp9600SetSensorConfig(0, MCP9600_DEVICE_ID, &scfg);
  mcp9600SetDeviceConfig(0, MCP9600_DEVICE_ID, &dcfg);
}

int get_temperature(void)
{
  unsigned char status;
  int t, rc;

  rc = mcp9600GetStatus(0, MCP9600_DEVICE_ID, &status);
  if (rc)
    return rc;
  if (status & MCP9600_STATUS_RANGE)
    return 300;
  if (status & MCP9600_STATUS_SHORT_CIRCUIT)
    return 400;
  rc = mcp9600GetHotJunctionTemperature(0, MCP9600_DEVICE_ID, &t);
  if (rc)
    return rc;
  return t / 100;
}
