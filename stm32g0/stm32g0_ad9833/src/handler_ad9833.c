#include "board.h"
#ifdef DDS_TYPE_AD9833
#include <ad9833.h>
#include <generic_dds.h>
#include "handler.h"

unsigned short ad9833_cfg, ad9833_divider;

const unsigned char device_id = I2C_ADDRESS;
const DdsConfig device_config =
{
    .max_attenuator = 0,
    .level_meter_type = LEVEL_METER_AD8307,
    .mclk = 25000000,
    .max_mv = 3300,
    .type = DDS_AD9833
};

void handler_init(void)
{
    ad9833_cfg = AD9833_CFG_SLEEP1|AD9833_CFG_SLEEP12;
    ad9833_set_config(0, ad9833_cfg);
    ad9833_divider = 1;
}

void set_frequency_code(int channel, unsigned long long int code, unsigned short divider)
{
    ad9833_set_freq0_word(0, (unsigned long)code);
    if (ad9833_cfg & AD9833_CFG_MSBDIV2)
    {
        ad9833_divider = divider;
        if (ad9833_divider == 1)
        {
            if ((ad9833_cfg & AD9833_CFG_MSB) == AD9833_CFG_MSBDIV2)
                ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_MSB);
        }
        else if ((ad9833_cfg & AD9833_CFG_MSB) == AD9833_CFG_MSB)
            ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_MSBDIV2);
    }
}

void set_mode(int channel, int mode)
{
    switch (mode)
    {
        case DDS_MODE_SINE:
            ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_SINUS);
            break;
        case DDS_MODE_TRIANGLE:
            ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_TRIANGLE);
            break;
        case DDS_MODE_SQUARE:
            if (ad9833_divider == 1)
                ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_MSB);
            else
                ad9833_update_mode(0, &ad9833_cfg, AD9833_CFG_MSBDIV2);
            break;
        default:
            break;
    }
}

void enable_output(int channel, int enable)
{
    if (enable)
        ad9833_cfg &= ~(AD9833_CFG_SLEEP1|AD9833_CFG_SLEEP12);
    else
        ad9833_cfg |= AD9833_CFG_SLEEP1|AD9833_CFG_SLEEP12;
    ad9833_set_config(0, ad9833_cfg);
}

void set_frequency(int channel, unsigned long long int frequency)
{
}

void set_attenuator(int channel, unsigned int value)
{
}
#endif
