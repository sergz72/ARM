#ifndef _ADF4351_H_
#define _ADF4351_H_

#define ADF4351_MAX_FREQUENCY 4400000000L
#define ADF4351_MIN_FREQUENCY   35000000L

typedef struct
{
    unsigned int refin;
    int low_spur_mode;
    int reference_doubler;
    int reference_divide_by_2;
    unsigned int r_counter;
    int cycle_slip_reduction;
    unsigned int charge_pump_current;
} ADF4351Config;

void adf4351_power_down(int channel);
void adf4351_power_up(int channel);
void adf4351_output_enable(int channel, int aux, int enable);
int adf4351_set_freq(int channel, unsigned long long freq);
void adf4351_set_rf_power(int channel, int aux, unsigned int value);
void adf4351_init(int channel, const ADF4351Config *config);
void adf4351_set_r_counter(int channel, unsigned int value);
void adf4351_write(int channel, unsigned int data); // should be defined in hal.c

#endif
