#include "board.h"
#ifdef DDS_TYPE_ADF4351
#include <adf4351.h>
#include <generic_dds.h>
#include "handler.h"

#define R_COUNTER_BASE 156

const unsigned char device_id = I2C_ADDRESS;
const DdsConfig device_config =
{
    .max_attenuator = 9,
    .level_meter_type = LEVEL_METER_AD8318,
    .mclk = 0,
    .max_mv = 3300,
    .type = DDS_ADF4351
};

const ADF4351Config adf4351_config =
{
    .refin = 10000000,
    .reference_divide_by_2 = 0,
    .reference_doubler = 0,
    .cycle_slip_reduction = 0,
    .low_spur_mode = 0,
    .r_counter = R_COUNTER_BASE,
    .charge_pump_current = 2500 // uA
};

void handler_init(void)
{
    adf4351_init(0, &adf4351_config);
    adf4351_output_enable(0, 0, 1);
    adf4351_output_enable(0, 1, 1);
    adf4351_set_rf_power(0, 1, 3); // max power on aux
}

void set_frequency_code(int channel, unsigned long long int code, unsigned short divider)
{
}

void set_mode(int channel, int mode)
{
}

void enable_output(int channel, int enable)
{
  if (enable)
      adf4351_power_up(0);
  else
      adf4351_power_down(0);
}

void set_frequency(int channel, unsigned long long int frequency)
{
    if (frequency > 64000ULL * 65535ULL)
        adf4351_set_r_counter(0, R_COUNTER_BASE / 2);
    else
        adf4351_set_r_counter(0, R_COUNTER_BASE);
    adf4351_set_freq(0, frequency);
}

void set_attenuator(int channel, unsigned int value)
{
    unsigned int d = 3;
    if (value >= 9)
        d = 0;
    else if (value >= 6)
        d = 1;
    else if (value >= 3)
        d = 2;
    adf4351_set_rf_power(0, 0, d);
}
#endif
