#include "board.h"
#ifdef DDS_TYPE_AD9850
#include <ad9850.h>
#include <generic_dds.h>
#include "handler.h"

const unsigned char device_id = I2C_ADDRESS;
const DdsConfig device_config =
{
    .max_attenuator = 0,
    .level_meter_type = LEVEL_METER_AD8307,
    .mclk = 125000000,
    .max_mv = 3300,
    .type = DDS_AD9850
};

void handler_init(void)
{
    ad9850_init(0, 1, 0);
}

void set_frequency_code(int channel, unsigned long long int code, unsigned short divider)
{
  ad9850_set_freq_word(0, (unsigned long)code, 0);
}

void set_mode(int channel, int mode)
{
}

void enable_output(int channel, int enable)
{
  if (!enable)
      ad9850_power_down(0);
}

void set_frequency(int channel, unsigned long long int frequency)
{
}

void set_attenuator(int channel, unsigned int value)
{
}

#endif
