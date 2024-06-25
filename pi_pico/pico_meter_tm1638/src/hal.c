#include <hardware/clocks.h>
#include "board.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include <pwm.pio.h>
#include <counter.pio.h>
#include <cap.pio.h>
#include <spi_3wire_soft.h>
#include <pico/time.h>

volatile unsigned int counter_value, cap_value, cap_value_updated;

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

void pwm_pio_init(PIO pio, uint sm)
{
  uint offset = pio_add_program(pio, &pwm_program);
  pwm_program_init(pio, sm, offset, PICO_DEFAULT_LED_PIN);
  // 1 second gate low (3 cycles pio program length) period = 125000000 / 3
  pio_pwm_set_period(pio, sm, PWM_PERIOD + PWM_PERIOD / 100);
  // level = 1% of period
  pio_pwm_set_level(pio, sm, PWM_PERIOD / 100);
}

void pio_irh()
{
  if (pio0_hw->irq & 1) // PIO0 IRQ0 fired
  {
    counter_value = UINT32_MAX - pio_sm_get(pio0, 1);
    pio0_hw->irq = 1;
  }
  else  // PIO0 IRQ1 fired
  {
    cap_value = CAP_MAX - pio_sm_get(pio0, 2);
    if (cap_value < CAP_OFFSET)
      cap_value = 0;
    else
      cap_value -= CAP_OFFSET;
    pio0_hw->irq = 2;
    gpio_set_dir(CAP_RLOW_PIN, true);
    cap_value_updated = 1;
  }
}

void pio_irq_setup(void)
{
  // Enable IRQ0 on PIO
  irq_set_exclusive_handler(PIO0_IRQ_0, pio_irh);
  irq_set_enabled(PIO0_IRQ_0, true);
  pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
}

void counter_pio_init(PIO pio, uint sm)
{
  uint offset = pio_add_program(pio, &counter_program);
  counter_program_init(pio, sm, offset, INPUT_PIN, PICO_DEFAULT_LED_PIN);
  pio_sm_put_blocking(pio, sm, UINT32_MAX);
  pio_sm_exec(pio, sm, pio_encode_pull(false, false));
  pio_sm_set_enabled(pio, sm, true);
}

void cap_pio_init(PIO pio, uint sm)
{
  uint offset = pio_add_program(pio, &cap_program);
  cap_program_init(pio, sm, offset, CAP_RHIGH_PIN, CAP_IN_PIN);
}

void SystemInit(void)
{
  unsigned int gen_slice_num, channel;

  gpio_init(GEN_PIN);
  gpio_set_dir(GEN_PIN, GPIO_OUT);

  // Generator PWM Config
  gpio_set_function(GEN_PIN, GPIO_FUNC_PWM);
  gen_slice_num = pwm_gpio_to_slice_num(GEN_PIN);
  channel = pwm_gpio_to_channel(GEN_PIN);
  pwm_set_wrap(gen_slice_num, 100);
  pwm_set_chan_level(gen_slice_num, channel, 50);
  pwm_set_clkdiv(gen_slice_num,1);
  pwm_set_enabled(gen_slice_num, true);

  counter_value = cap_value = cap_value_updated = 0;

  pio_irq_setup();
  pwm_pio_init(pio0, 0);
  counter_pio_init(pio0, 1);

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

  gpio_init(CAP_RLOW_PIN);
  gpio_put(CAP_RLOW_PIN, false);
  gpio_set_dir(CAP_RLOW_PIN, true);
  gpio_disable_pulls(CAP_IN_PIN);
  gpio_disable_pulls(CAP_RLOW_PIN);

  cap_pio_init(pio0, 2);
}

void spi3_delay(void)
{
  sleep_us(1);
}
