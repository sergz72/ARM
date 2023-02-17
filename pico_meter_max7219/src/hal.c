#include <hardware/clocks.h>
#include "board.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include <pwm.pio.h>
#include <counter1.pio.h>
#include <counter2.pio.h>
#include <cap.pio.h>
#include <pico/time.h>
#include <spi_4bit_soft.h>
#include "dev_si5351.h"
#include <max7219.h>
#include <keyboard.h>
#include <i2c_soft.h>

volatile unsigned int counter_value1, counter_value2, cap_value, cap_value_updated;
unsigned int pwm_slice_num[2], pwm_channel[2];

// Write `period` to the input shift register
void pio_pwm_set_period(PIO pio, uint sm, unsigned int period) {
  pio_sm_set_enabled(pio, sm, false);
  pio_sm_put_blocking(pio, sm, period);
  pio_sm_exec(pio, sm, pio_encode_pull(false, false));
  pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
  pio_sm_set_enabled(pio, sm, true);
}

// Write `level` to TX FIFO. State machine will copy this into X.
void pio_pwm_set_level(PIO pio, uint sm, unsigned int level) {
  pio_sm_put_blocking(pio, sm, level);
}

void pwm_pio_init(PIO pio, uint sm, unsigned int period)
{
  uint offset = pio_add_program(pio, &pwm_program);
  pwm_program_init(pio, sm, offset, PICO_DEFAULT_LED_PIN);
  // 1 second gate low (3 cycles pio program length) period = 125000000 / 3
  pio_pwm_set_period(pio, sm, period + period / 100);
  // level = 1% of period
  pio_pwm_set_level(pio, sm, period / 100);
}

void pio0_irh()
{
  if (pio0_hw->irq & 1) // PIO0 IRQ0 fired
  {
    counter_value1 = UINT32_MAX - pio_sm_get(pio0, 1);
    pio0_hw->irq = 1;
    return;
  }
  counter_value2 = UINT32_MAX - pio_sm_get(pio0, 2);
  pio0_hw->irq = 2;
}

void pio1_irh()
{
  cap_value = CAP_MAX - pio_sm_get(pio0, 3);
  if (cap_value < CAP_OFFSET)
    cap_value = 0;
  else
    cap_value -= CAP_OFFSET;
  pio0_hw->irq = 0;
  gpio_set_dir(CAP_RLOW_PIN, true);
  cap_value_updated = 1;
}

void pio_irq_setup(void)
{
  // Enable IRQ0 on PIO
  irq_set_exclusive_handler(PIO0_IRQ_0, pio0_irh);
  irq_set_enabled(PIO0_IRQ_0, true);
  pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
  irq_set_exclusive_handler(PIO1_IRQ_0, pio1_irh);
  irq_set_enabled(PIO1_IRQ_0, true);
  pio1_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS;
}

void counter_pio_init(PIO pio, uint sm, uint sm2)
{
  uint offset1 = pio_add_program(pio, &counter1_program);
  uint offset2 = pio_add_program(pio, &counter2_program);
  counter1_program_init(pio, sm, offset1, FREQ1_PIN, PICO_DEFAULT_LED_PIN);
  counter2_program_init(pio, sm2, offset2, FREQ2_PIN, PICO_DEFAULT_LED_PIN);
  pio_sm_put_blocking(pio, sm, UINT32_MAX);
  pio_sm_exec(pio, sm, pio_encode_pull(false, false));
  pio_sm_set_enabled(pio, sm, true);
  pio_sm_put_blocking(pio, sm2, UINT32_MAX);
  pio_sm_exec(pio, sm2, pio_encode_pull(false, false));
  pio_sm_set_enabled(pio, sm2, true);
}

void cap_pio_init(PIO pio, uint sm)
{
  uint offset = pio_add_program(pio, &cap_program);
  cap_program_init(pio, sm, offset, CAP_RHIGH_PIN, CAP_IN_PIN);
}

void KbdGpioInit(void)
{
  gpio_init(KB1_PIN);
  gpio_init(KB2_PIN);
  gpio_init(KB3_PIN);
  gpio_init(KB4_PIN);
  gpio_set_dir(KB1_PIN, false);
  gpio_set_dir(KB2_PIN, false);
  gpio_set_dir(KB3_PIN, false);
  gpio_set_dir(KB4_PIN, false);
  gpio_pull_up(KB1_PIN);
  gpio_pull_up(KB2_PIN);
  gpio_pull_up(KB3_PIN);
  gpio_pull_up(KB4_PIN);
}

void SystemInit(void)
{
  gpio_init(PWM1_PIN);
  gpio_set_dir(PWM1_PIN, GPIO_OUT);
  gpio_init(PWM2_PIN);
  gpio_set_dir(PWM2_PIN, GPIO_OUT);

  // Generator PWM Config
  gpio_set_function(PWM1_PIN, GPIO_FUNC_PWM);
  gpio_set_function(PWM2_PIN, GPIO_FUNC_PWM);
  pwm_slice_num[0] = pwm_gpio_to_slice_num(PWM1_PIN);
  pwm_channel[0] = pwm_gpio_to_channel(PWM1_PIN);
  pwm_slice_num[1] = pwm_gpio_to_slice_num(PWM2_PIN);
  pwm_channel[1] = pwm_gpio_to_channel(PWM2_PIN);

  counter_value1 = counter_value2 = cap_value = cap_value_updated = 0;

  pio_irq_setup();
  pwm_pio_init(pio1, 0, 125000000 / 3);
  counter_pio_init(pio0, 0, 1);

  gpio_init(SPI_4BIT_DIO0_PIN);
  gpio_init(SPI_4BIT_DIO1_PIN);
  gpio_init(SPI_4BIT_DIO2_PIN);
  gpio_init(SPI_4BIT_DIO3_PIN);
  gpio_init(SPI_4BIT_CLK_PIN);
  gpio_init(SPI_4BIT_CS_PIN);
  gpio_put(SPI_4BIT_CS_PIN, true);
  gpio_put(SPI_4BIT_CLK_PIN, false);
  gpio_set_dir(SPI_4BIT_DIO0_PIN, true);
  gpio_set_dir(SPI_4BIT_DIO1_PIN, true);
  gpio_set_dir(SPI_4BIT_DIO2_PIN, true);
  gpio_set_dir(SPI_4BIT_DIO3_PIN, true);
  gpio_set_dir(SPI_4BIT_CLK_PIN, true);
  gpio_set_dir(SPI_4BIT_CS_PIN, true);

  gpio_init(CAP_RLOW_PIN);
  gpio_put(CAP_RLOW_PIN, false);
  gpio_set_dir(CAP_RLOW_PIN, true);
  gpio_disable_pulls(CAP_IN_PIN);
  gpio_disable_pulls(CAP_RLOW_PIN);

  cap_pio_init(pio1, 1);

  KbdGpioInit();
}

void spi_4bit_delay(void)
{
  sleep_us(1);
}

void SPI_4BIT_DATA_SET(unsigned char data)
{
  gpio_put_masked(0x0F << SPI_4BIT_DIO0_PIN, data << SPI_4BIT_DIO0_PIN);
}

void max7219_send(unsigned short **data)
{
  spi_4bit_send16(data, MAX7219_NUM_PARALLEL, MAX7219_NUM_SERIES);
}

int i2c_soft_command(int channel, unsigned int address, const unsigned char *commands,  unsigned int commands_length,
                     const unsigned char *out_data, unsigned int out_data_length,
                     unsigned char in_data[], unsigned int in_data_length, unsigned int timeout)
{
  return 1;
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
  return 0;//i2c_soft_read(channel, address, data, l, I2C_TIMEOUT);
}

int inaWriteRegister(int channel, unsigned char address, unsigned char reg, unsigned short data)
{
  unsigned char d[2];
  d[0] = data >> 8;
  d[1] = data & 0xFF;
  return i2c_soft_command(channel, address, &reg, 1, d, 2, NULL, 0, I2C_TIMEOUT);
}

int mcp3421Write(int channel, unsigned char address, unsigned char data)
{
  return i2c_soft_command(channel, address, NULL, 0, &data, 1, NULL, 0, I2C_TIMEOUT);
}

int si5351_write_bulk(int channel, unsigned char addr, unsigned char bytes, unsigned char *data)
{
  return i2c_soft_command(channel, SI5351_DEVICE_ID, &addr, 1, data, bytes, NULL, 0, I2C_TIMEOUT);
}

int si5351_write(int channel, unsigned char addr, unsigned char data)
{
  return i2c_soft_command(channel, SI5351_DEVICE_ID, &addr, 1, &data, 1, NULL, 0, I2C_TIMEOUT);
}

void KbdSet(int state)
{
  SPI_4BIT_DATA_SET(state);
}

int KbdGet(void)
{
  unsigned int value = gpio_get_all();
  // 22, 2, 3, 4
  return (int)(((value >> 22) & 1) | ((value & 0x1C) >> 1));
}

unsigned char HALKbdHandler(void)
{
  return MatrixKbdHandler(0);
}

void KbdLocksUpdated(int Locks)
{
}
