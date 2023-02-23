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
#include "settings.h"
#include "dev_voltmeter.h"
#include "dev_cap_meter.h"
#include "dev_freq_pwm.h"
#include "hardware/adc.h"
#include <memory.h>
#include <hardware/sync.h>
#include <hardware/flash.h>
#include <malloc.h>
//#include <stdio.h>

#define FLASH_TARGET_OFFSET (2048 * 1024 - FLASH_SECTOR_SIZE)

volatile unsigned int counter_value1, counter_value2, cap_value, cap_value_updated;
unsigned int pwm_slice_num[2], pwm_channel[2], pwm_program_offset;

// Write `period` to the input shift register
void pio_pwm_set_params(PIO pio, uint sm, unsigned int period, unsigned int level) {
  pio_sm_set_enabled(pio, sm, false);
  pio_sm_put_blocking(pio, sm, period + level);
  pio_sm_exec(pio, sm, pio_encode_pull(false, false));
  pio_sm_exec(pio, sm, pio_encode_out(pio_isr, 32));
  pio_sm_exec(pio, sm, pio_encode_jmp(pwm_program_offset));
  pio_sm_set_enabled(pio, sm, true);
  pio_sm_put_blocking(pio, sm, level);
}

void pwm_pio_init(PIO pio, uint sm)
{
  pwm_program_offset = pio_add_program(pio, &pwm_program);
  pwm_program_init(pio, sm, pwm_program_offset, PICO_DEFAULT_LED_PIN);
}

void pio0_irh()
{
  //printf("pio0_irh\n");
  if (pio0_hw->irq & 1) // PIO0 IRQ0 fired
  {
    counter_value1 = UINT32_MAX - pio_sm_get(COUNTER_PIO, COUNTER1_SM);
    pio0_hw->irq = 1;
    return;
  }
  counter_value2 = UINT32_MAX - pio_sm_get(COUNTER_PIO, COUNTER2_SM);
  pio0_hw->irq = 2;
}

void pio1_irh()
{
  //printf("pio1_irh\n");
  cap_value = settings[SETTING_CAP_MAX] - pio_sm_get(CAP_PIO, CAP_SM);
  if (cap_value < settings[SETTING_CAP_OFFSET])
    cap_value = 0;
  else
    cap_value -= settings[SETTING_CAP_OFFSET];
  pio1_hw->irq = 0;
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
  //pio_sm_set_enabled(pio, sm, true);
  pio_sm_put_blocking(pio, sm2, UINT32_MAX);
  pio_sm_exec(pio, sm2, pio_encode_pull(false, false));
  //pio_sm_set_enabled(pio, sm2, true);
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
  pwm_slice_num[0] = pwm_gpio_to_slice_num(PWM1_PIN);
  pwm_channel[0] = pwm_gpio_to_channel(PWM1_PIN);
  pwm_slice_num[1] = pwm_gpio_to_slice_num(PWM2_PIN);
  pwm_channel[1] = pwm_gpio_to_channel(PWM2_PIN);
  pwm_set_output_polarity(pwm_slice_num[0], false, false);
  pwm_set_output_polarity(pwm_slice_num[1], false, false);

  counter_value1 = counter_value2 = cap_value = cap_value_updated = 0;

  pio_irq_setup();
  pwm_pio_init(PWM_PIO, PWM_SM);
  counter_pio_init(COUNTER_PIO, COUNTER1_SM, COUNTER2_SM);

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

  cap_pio_init(CAP_PIO, CAP_SM);

  KbdGpioInit();

  adc_init();
  adc_set_clkdiv(0);
  adc_gpio_init(ADC0_PIN);
  adc_gpio_init(ADC1_PIN);
  adc_gpio_init(ADC2_PIN);
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

int i2c_soft_read(int channel, unsigned int address, unsigned char *in_data, unsigned int in_data_length, unsigned int timeout)
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

int hal_read_settings(void)
{
  memcpy(settings, (void*)(XIP_BASE + FLASH_TARGET_OFFSET), SETTINGS_SIZE);
  return 0;
}

int save_settings(void)
{
  unsigned char *buffer;
  unsigned int ints;
  buffer = calloc(1, FLASH_PAGE_SIZE);
  if (buffer)
  {
    memcpy(buffer, settings, SETTINGS_SIZE);
    ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, buffer, FLASH_PAGE_SIZE);
    restore_interrupts(ints);
    free(buffer);
    return 0;
  }
  return 1;
}

void adc_start_conversion(int channel)
{
  adc_select_input(channel);
  hw_set_bits(&adc_hw->cs, ADC_CS_START_ONCE_BITS);
}

int adc_get_result(void)
{
  return adc_hw->result;
}

void cap_meter_init(void)
{
  if (cap_value_updated)
  {
    //puts("cap_value_updated");
    cap_value_updated = 0;
    gpio_set_dir(CAP_RLOW_PIN, false);
    pio_sm_put_blocking(CAP_PIO, CAP_SM, settings[SETTING_CAP_MAX]);
  }
}

void cap_meter_on(void)
{
  pio_sm_clear_fifos(CAP_PIO, CAP_SM);
  gpio_set_dir(CAP_RLOW_PIN, false);
  pio_sm_put_blocking(CAP_PIO, CAP_SM, settings[SETTING_CAP_MAX]);
  pio_sm_set_enabled(CAP_PIO, CAP_SM, true);
}

void cap_meter_off(void)
{
  pio_sm_set_enabled(CAP_PIO, CAP_SM, false);
}

void freq_pwm_on(unsigned int interval)
{
  freq_counter_set_interval(interval);
  pio_sm_set_enabled(COUNTER_PIO, COUNTER1_SM, true);
  pio_sm_set_enabled(COUNTER_PIO, COUNTER2_SM, true);
}

void freq_pwm_off(void)
{
  pio_sm_set_enabled(PWM_PIO, PWM_SM, false);
  pio_sm_set_enabled(COUNTER_PIO, COUNTER1_SM, false);
  pio_sm_set_enabled(COUNTER_PIO, COUNTER2_SM, false);
}

void pwm_on(int channel)
{
  gpio_set_function(PWM1_PIN, GPIO_FUNC_PWM);
  gpio_set_function(PWM2_PIN, GPIO_FUNC_PWM);
  pwm_set_enabled(pwm_slice_num[channel], true);
}

void pwm_off(int channel)
{
  pwm_set_enabled(pwm_slice_num[channel], false);
  gpio_init(PWM1_PIN);
  gpio_init(PWM2_PIN);
  pwm_set_counter(pwm_slice_num[channel], 0);
}

void pwm_set_freq(int channel, unsigned int freq, unsigned int duty)
{
  unsigned int hz, div, wrap;

  hz = clock_get_hz(clk_sys);
  div = ((hz / freq) >> 16) + 1;
  wrap = hz / div / freq;
  duty = duty * wrap / 1000;
  pwm_set_clkdiv(pwm_slice_num[channel], (float) div);
  pwm_set_wrap(pwm_slice_num[channel], wrap);
  pwm_set_chan_level(pwm_slice_num[channel], pwm_channel[channel], duty);
}

void freq_counter_set_interval(unsigned int interval)
{
  unsigned int period = interval * 125000000 / 3;
  // level = 1% of period
  unsigned int l = period / 100;
  // 1 second gate low (3 cycles pio program length) period = 125000000 / 3
  pio_pwm_set_params(PWM_PIO, PWM_SM, period, l);
}

unsigned int get_counter_value1(void)
{
  return counter_value1;
}

unsigned int get_counter_value2(void)
{
  return counter_value2;
}
