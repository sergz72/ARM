#include "board.h"
#include <i2c_soft.h>
#include <ina226.h>
#include <mcp3421.h>
#include <ads1115.h>
#include <generic_dds.h>
#include <stdio.h>
#include <hardware/clocks.h>
#include <hardware/pio.h>
#include <pico/stdio.h>
#include "dev_si5351.h"
#include <pico/time.h>
#include <pwm.pio.h>
#include <counter.pio.h>

#define PWM_PIO pio0
#define GATE_SM 0
#define PWM4_0_SM 1
#define PWM5_0_SM 2
#define PWM5_1_SM 3
#define COUNTER_PIO pio1

static bool led_state;
static int pwm_program_offset, counter_program_offset;
static unsigned int pwm_frequency_40, pwm_frequency_50, pwm_frequency_51;
static unsigned int pwm_duty_40, pwm_duty_50, pwm_duty_51;

// Write `period` to the input shift register
void pio_pwm_set_params(PIO pio, uint sm, unsigned int period, unsigned int level) {
  pio_sm_set_enabled(pio, sm, false);
  pio_sm_put_blocking(pio, sm, period);
  pio_sm_exec(pio, sm, pio_encode_pull(false, false));
  pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
  pio_sm_exec(pio, sm, pio_encode_jmp(pwm_program_offset));
  pio_sm_set_enabled(pio, sm, true);
  pio_sm_put_blocking(pio, sm, level);
}

void  counter_pio_init(unsigned int sm, unsigned int pin)
{
  counter_program_init(COUNTER_PIO, sm, counter_program_offset, pin, PIN_GATE);
  pio_sm_put_blocking(COUNTER_PIO, sm, UINT32_MAX);
  pio_sm_exec(COUNTER_PIO, sm, pio_encode_pull(false, false));
  pio_sm_set_enabled(COUNTER_PIO, sm, true);
}

int counter_get(int module_id, int pin_id)
{
  int value = -1;
  unsigned int sm;

  switch (module_id)
  {
    case 4:
      if (pin_id == 0)
        sm = COUNTER4_0_SM;
      else
        return value;
      break;
    case 5:
      sm = pin_id == 0 ? COUNTER5_0_SM : COUNTER5_1_SM;
      break;
    default:
      return value;
  }
  while (pio_sm_get_rx_fifo_level(COUNTER_PIO, sm))
    value = UINT32_MAX - pio_sm_get(COUNTER_PIO, sm);
  return value;
}

int pwm_enable(int module_id, int pin_id, int enable)
{
  switch (module_id)
  {
    case 4:
      if (pin_id == 0)
      {
        if (enable)
        {
          pwm_program_init(PWM_PIO, PWM4_0_SM, pwm_program_offset, PIN_4_0);
          pwm_set_frequency_and_duty(module_id, pin_id, pwm_frequency_40, pwm_duty_40);
        }
        else
        {
          pio_sm_set_enabled(PWM_PIO, PWM4_0_SM, false);
          gpio_init(PIN_4_0);
        }
      }
      else
        return 1;
      break;
    case 5:
      if (pin_id == 0)
      {
        if (enable)
        {
          pwm_program_init(PWM_PIO, PWM5_0_SM, pwm_program_offset, PIN_5_0);
          pwm_set_frequency_and_duty(module_id, pin_id, pwm_frequency_50, pwm_duty_50);
        }
        else
        {
          pio_sm_set_enabled(PWM_PIO, PWM5_0_SM, false);
          gpio_init(PIN_5_0);
        }
      }
      else
      {
        if (enable)
        {
          pwm_program_init(PWM_PIO, PWM5_1_SM, pwm_program_offset, PIN_5_1);
          pwm_set_frequency_and_duty(module_id, pin_id, pwm_frequency_51, pwm_duty_51);
        }
        else
        {
          pio_sm_set_enabled(PWM_PIO, PWM5_1_SM, false);
          gpio_init(PIN_5_1);
        }
      }
      break;
    default:
      return 1;
  }
  return 0;
}

int pwm_set_frequency_and_duty(int module_id, int pin_id, unsigned int frequency, unsigned int duty)
{
  switch (module_id)
  {
    case 4:
      if (pin_id == 0)
      {
        pwm_frequency_40 = frequency;
        pwm_duty_40 = duty;
        pio_pwm_set_params(PWM_PIO, PWM4_0_SM, frequency, duty);
      }
      else
        return 1;
      break;
    case 5:
      if (pin_id == 0)
      {
        pwm_frequency_50 = frequency;
        pwm_duty_50 = duty;
        pio_pwm_set_params(PWM_PIO, PWM5_0_SM, frequency, duty);
      }
      else
      {
        pwm_frequency_51 = frequency;
        pwm_duty_51 = duty;
        pio_pwm_set_params(PWM_PIO, PWM5_1_SM, frequency, duty);
      }
      break;
    default:
      return 1;
  }
  return 0;
}

static void configure_gate_pin(void)
{
  pwm_program_init(PWM_PIO, GATE_SM, pwm_program_offset, PIN_GATE);
  unsigned int period = clock_get_hz(clk_sys) / 3;
  // level = 1% of period
  unsigned int l = period / 100;
  // 1 second gate low (3 cycles pio program length) period = 125000000 / 3
  pio_pwm_set_params(PWM_PIO, GATE_SM, period + l, l);
}

void configure_ports(void)
{
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
  gpio_init(PIN_INTERRUPT);
  gpio_set_dir(PIN_SCLK, GPIO_IN);
  gpio_pull_up(PIN_INTERRUPT);

  pwm_program_offset = pio_add_program(PWM_PIO, &pwm_program);
  configure_gate_pin();
  counter_program_offset = pio_add_program(COUNTER_PIO, &counter_program);

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
  gpio_pull_up(PIN_SDI0);
  gpio_pull_up(PIN_SDI1);

  gpio_init(PIN_SDO0);
  gpio_set_dir(PIN_SDO0, GPIO_OUT);
  gpio_init(PIN_SDO1);
  gpio_set_dir(PIN_SDO1, GPIO_OUT);
}

void configure_i2c(void)
{
  gpio_init(PIN_I2C_SDA_OE);
  gpio_init(PIN_I2C_SCL_OE);
  gpio_init(PIN_I2C_SDA_IN);
  gpio_init(PIN_I2C_SCL_IN);
  gpio_init(PIN_SDA4);
  gpio_init(PIN_SCL4);
  gpio_init(PIN_SDA5);
  gpio_init(PIN_SCL5);
  i2c_soft_init(0);
  gpio_put(PIN_SCL4, false);
  gpio_put(PIN_SDA4, false);
  gpio_put(PIN_SCL5, false);
  gpio_put(PIN_SDA5, false);
  i2c_soft_init(4);
  i2c_soft_init(5);
  gpio_set_dir(PIN_I2C_SDA_OE, GPIO_OUT);
  gpio_set_dir(PIN_I2C_SCL_OE, GPIO_OUT);
  gpio_set_dir(PIN_I2C_SDA_IN, GPIO_IN);
  gpio_set_dir(PIN_I2C_SCL_IN, GPIO_IN);
  gpio_pull_up(PIN_SDA4);
  gpio_pull_up(PIN_SCL4);
  gpio_pull_up(PIN_SDA5);
  gpio_pull_up(PIN_SCL5);
  gpio_pull_up(PIN_I2C_SDA_IN);
  gpio_pull_up(PIN_I2C_SCL_IN);
}

void change_channel(int channel)
{
  gpio_put(PIN_DEVICE_ID0, channel & 1);
  gpio_put(PIN_DEVICE_ID1, channel & 2);
  gpio_put(PIN_DEVICE_ID2, channel & 4);
}

void SPI_2BIT_DATA_SET(unsigned int data)
{
  gpio_put(PIN_SDO0, data & 0x20000000);
  gpio_put(PIN_SDO1, data & 0x40000000);
}

unsigned int spi2_data_get(void)
{
  unsigned int data = gpio_get(PIN_SDI0) ? 1 : 0;
  if (gpio_get(PIN_SDI1))
    data |= 2;
  return data;
}

void SCL_HIGH(int channel)
{
  switch (channel)
  {
    case 4:
      gpio_set_dir(PIN_SCL4, GPIO_IN);
      break;
    case 5:
      gpio_set_dir(PIN_SCL5, GPIO_IN);
      break;
    default:
      gpio_put(PIN_I2C_SCL_OE, true);
      break;
  }
}

void SCL_LOW(int channel)
{
  switch (channel)
  {
    case 4:
      gpio_set_dir(PIN_SCL4, GPIO_OUT);
      break;
    case 5:
      gpio_set_dir(PIN_SCL5, GPIO_OUT);
      break;
    default:
      gpio_put(PIN_I2C_SCL_OE, false);
      break;
  }
}

void SDA_HIGH(int channel)
{
  switch (channel)
  {
    case 4:
      gpio_set_dir(PIN_SDA4, GPIO_IN);
    break;
    case 5:
      gpio_set_dir(PIN_SDA5, GPIO_IN);
    break;
    default:
      gpio_put(PIN_I2C_SDA_OE, true);
    break;
  }
}

void SDA_LOW(int channel)
{
  switch (channel)
  {
    case 4:
      gpio_set_dir(PIN_SDA4, GPIO_OUT);
    break;
    case 5:
      gpio_set_dir(PIN_SDA5, GPIO_OUT);
    break;
    default:
      gpio_put(PIN_I2C_SDA_OE, false);
    break;
  }
}

int SDA_IN(int channel)
{
  switch (channel)
  {
    case 4:
      return gpio_get(PIN_SDA4);
    case 5:
      return gpio_get(PIN_SDA5);
    default:
      return gpio_get(PIN_I2C_SDA_IN);
  }
}

int SCL_IN(int channel)
{
  switch (channel)
  {
    case 4:
      return gpio_get(PIN_SCL4);
    case 5:
      return gpio_get(PIN_SCL5);
    default:
      return gpio_get(PIN_I2C_SCL_IN);
  }
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

int dds_command(unsigned char deviceId, unsigned char cmd, dds_cmd *data, int idx, void *config)
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
      c.c3.command = cmd;
      c.c3.channel = data->channel;
      c.c3.parameter = data->set_attenuator_command.attenuator_value;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 3,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    case DDS_COMMAND_SET_FREQUENCY:
    case DDS_COMMAND_SET_FREQUENCY_CODE:
      c.c12.command = cmd;
      c.c12.channel = data->channel;
      c.c12.freq = data->set_frequency_command.frequency;
      c.c12.div = data->set_frequency_command.divider;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 8,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    case DDS_COMMAND_SET_MODE:
      c.c3.command = cmd;
      c.c3.channel = data->channel;
      c.c3.parameter = data->set_mode_command.mode;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 3,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    case DDS_COMMAND_SWEEP:
    case DDS_COMMAND_SWEEP_CODES:
      c.c20.command = cmd;
      c.c20.channel = data->channel;
      c.c20.freq = data->sweep_command.frequency;
      c.c20.step = data->sweep_command.step;
      c.c20.points = data->sweep_command.points;
      c.c20.div = data->sweep_command.divider;
      return i2c_soft_command(idx, deviceId, (unsigned char*)&c, 8,
                              NULL, 0, NULL, 0, I2C_TIMEOUT);
    default:
      return 1;
  }
}

int si5351_write(unsigned char device_address, int channel, const unsigned char *data, unsigned int length)
{
  return i2c_soft_command(channel, device_address, NULL, 0, data, length, NULL, 0, I2C_TIMEOUT);
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

int get_interrupt_pin_level(void)
{
  return gpio_get(PIN_INTERRUPT);
}
