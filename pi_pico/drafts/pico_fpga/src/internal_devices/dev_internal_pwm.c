#include "board.h"
#include "dev_internal_pwm.h"
#include <devices.h>
#include <dev_pwm.h>
#include <pwm.pio.h>

static const PWMConfig config1 =
{
  .bits = 32,
  .channels = 1,
  .dds_clock = 0,
  .mclk = 50000000
};

static const PWMConfig config2 =
{
  .bits = 32,
  .channels = 2,
  .dds_clock = 0,
  .mclk = 50000000
};

static int pwm_enable_output(int idx, void *cfg, void *data, int channel, int enable);

static int pwm_set_frequency_duty(int idx, void *cfg, void *data, int channel, unsigned int frequency,
                            unsigned int duty);

static const dev_pwm dev =
{
  .cfg = NULL,
  .enable_output = pwm_enable_output,
  .set_frequency_and_duty = pwm_set_frequency_duty,
};

static int pwm_enable_output(int idx, void *cfg, void *data, int channel, int enable)
{
  return pwm_enable(idx, channel, enable);
}

static int pwm_set_frequency_duty(int idx, void *cfg, void *data, int channel, unsigned int frequency,
                            unsigned int duty)
{
  return pwm_set_frequency_and_duty(idx, channel, frequency, duty);
}

void* internal_pwm_initializer(int idx, void **data)
{
  pwm_initializer();
  return &dev;
}

int internal_pwm_save_config(int idx, void *buffer)
{
  return BuildPWMConfig(buffer, idx == 4 ? &config1 : &config2, "Internal PWM");
}
