#include "board.h"
#include "dev_internal_pwm.h"
#include <dev_pwm.h>
#include <pwm.pio.h>
#include <stdlib.h>
#include <string.h>

static const PWMConfig config =
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

void internal_pwm_initializer(DeviceObject *o)
{
  if (alloc_pio(o->idx))
  {
    o->device_config = NULL;
    return;
  }
  pwm_initializer();
  void *config = malloc(sizeof(dev_pwm));
  if (config)
    memcpy(config, &dev, sizeof(dev_pwm));
  o->device_config = config;
}

int internal_pwm_save_config(DeviceObject *o, void *buffer)
{
  return BuildPWMConfig(buffer, &config, "Internal PWM");
}
