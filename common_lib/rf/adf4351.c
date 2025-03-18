#include <adf4351.h>
#include "board.h"

typedef struct
{
    unsigned int registers[6];
    double reference_frequency;
    unsigned int refin;
    unsigned int r_counter;
    unsigned int mod;
    int reference_doubler;
    int reference_divide_by_2;
} ADF4351Channel;

ADF4351Channel channels[ADF4351_MAX_CHANNELS];

static void write_register(int channel, int reg)
{
    adf4351_write(channel, channels[channel].registers[reg]);
}

void adf4351_power_down(int channel)
{
    channels[channel].registers[2] |= 0x20; // power down
    write_register(channel, 2);
}

void adf4351_power_up(int channel)
{
    channels[channel].registers[2] &= ~0x20; // power up
    write_register(channel, 2);
}

int adf4351_set_freq(int channel, unsigned long long freq)
{
    if (freq > ADF4351_MAX_FREQUENCY || freq < ADF4351_MIN_FREQUENCY)
        return 1;
    unsigned int rfdiv = 0;
    while (freq < ADF4351_MAX_FREQUENCY / 2)
    {
        freq *= 2;
        rfdiv++;
    }
    unsigned int n = (unsigned int)((double)freq * channels[channel].mod / channels[channel].reference_frequency);
    unsigned int frac = n % channels[channel].mod;
    n /= channels[channel].mod;
    if (n < 75 || n > 65535)
        return 2;
    if (((channels[channel].registers[4] >> 20) & 7) != rfdiv)
    {
        channels[channel].registers[4] &= ~(7 << 20);
        channels[channel].registers[4] |= rfdiv << 20;
        write_register(channel, 4);
    }
    channels[channel].registers[0] = (n << 15) + (frac << 3);
    write_register(channel, 0);
    return 0;
}

static unsigned int build_register0(const ADF4351Config *config)
{
    return 65535 << 15; // int = 65535, frac = 0
}

static unsigned int build_register1(const ADF4351Config *config)
{
    return 1 + (1 << 15) + ((config->mod & 0xFFF) << 3) + (1 << 27); // phase = 1, prescaler 8/9
}

static unsigned int build_register2(const ADF4351Config *config)
{
    unsigned int current = config->charge_pump_current / 312 - 1;
    unsigned int value = ((current & 0x0F) << 9) |
                         ((config->r_counter & 0x3FF) << 14) |
                         0x22; // power down
    if (config->low_spur_mode)
        value |= 3 << 29;
    if (config->reference_doubler)
        value |= 1 << 25;
    if (config->reference_divide_by_2)
        value |= 1 << 24;
    if (config->pd_polarity_positive)
        value |= 1 << 6;
    return value;// | (1 << 4);
}

static unsigned int build_register3(const ADF4351Config *config)
{
    unsigned int value = 3 + (1 << 23) + (100 << 3);// + (1 << 15); // band select clock mode = high, clock divider = 100, fast lock enable
    if (config->cycle_slip_reduction)
        value |= 1 << 18;
    return value;
}

static unsigned int build_register4(int channel, const ADF4351Config *config)
{
    unsigned int band_select_clock_divider = channels[channel].reference_frequency / 60000;
    return 4 + (1 << 10) + (band_select_clock_divider << 12) + (1 << 23); // feedback is fundamental, mute till lock detect
}

static unsigned int build_register5(const ADF4351Config *config)
{
    return 5 + (3 << 19) + (1 << 22); // ld pin mode is digital lock detect
}

void calculate_reference_frequency(int channel)
{
    channels[channel].reference_frequency = (double)channels[channel].refin * (channels[channel].reference_doubler ? 2 : 1) /
                                            channels[channel].r_counter / (channels[channel].reference_divide_by_2 ? 2 : 1);
}

void adf4351_set_r_counter(int channel, unsigned int value)
{
    value &= 0x3FF;
    if (channels[channel].r_counter != value)
    {
        channels[channel].r_counter = value;
        calculate_reference_frequency(channel);
        channels[channel].registers[2] &= ~(0x3FF << 14);
        channels[channel].registers[2] |= value << 14;
        write_register(channel, 2);
    }
}

void adf4351_init(int channel, const ADF4351Config *config)
{
    channels[channel].refin = config->refin;
    channels[channel].reference_doubler = config->reference_doubler;
    channels[channel].reference_divide_by_2 = config->reference_divide_by_2;
    channels[channel].r_counter = config->r_counter;
    channels[channel].mod = config->mod;
    calculate_reference_frequency(channel);
    channels[channel].registers[0] = build_register0(config);
    channels[channel].registers[1] = build_register1(config);
    channels[channel].registers[2] = build_register2(config);
    channels[channel].registers[3] = build_register3(config);
    channels[channel].registers[4] = build_register4(channel, config);
    channels[channel].registers[5] = build_register5(config);
    for (int i = 5; i >= 0; i--)
        write_register(channel, i);
}

void adf4351_set_rf_power(int channel, int aux, unsigned int value)
{
    int n = aux ? 6 : 3;

    value &= 3;
    value <<= n;

    unsigned int mask = ~(3 << n);
    channels[channel].registers[4] &= mask;
    channels[channel].registers[4] |= value;

    write_register(channel, 4);
}

void adf4351_output_enable(int channel, int aux, int enable)
{
    int n = aux ? 8 : 5;

    if (enable)
        channels[channel].registers[4] |= (1 << n);
    else
        channels[channel].registers[4] &= ~(1 << n);

    write_register(channel, 4);
}
