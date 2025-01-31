#include <si5351.h>
#include <string.h>

#define SI5351_DEVICE_STATUS      0
#define SI5351_OUTPUT_ENABLE_CTRL 3
#define SI5351_CLK0_CTRL          16
#define SI5351_PLLA_PARAMETERS    26
#define SI5351_PLLB_PARAMETERS    34
#define SI5351_CLK0_PARAMETERS    42
#define SI5351_SSC_PARAM0         149
#define SI5351_CRYSTAL_LOAD       183
#define SI5351_FANOUT_ENABLE      187

#define SI5351_PARAMETERS_LENGTH  8

#define PLL_MAX_FREQUENCY 900000000
#define PLL_MIN_FREQUENCY 600000000

enum si5351_pll {SI5351_PLLA, SI5351_PLLB};

static int set_msn(si5351_dev *device, enum si5351_pll pll, unsigned int p1, unsigned int p2, unsigned int p3)
{
    unsigned char data[SI5351_PARAMETERS_LENGTH+1];
    data[0] = pll == SI5351_PLLA ? SI5351_PLLA_PARAMETERS : SI5351_PLLB_PARAMETERS;
    data[1] = (unsigned char)(p3 >> 8);
    data[2] = (unsigned char)p3;
    data[3] = (unsigned char)((p1 >> 16) & 3);
    data[4] = (unsigned char)(p1 >> 8);
    data[5] = (unsigned char)p1;
    data[6] = (unsigned char)(((p3 >> 12) & 0xF0) | ((p2 >> 16) & 0x0F));
    data[7] = (unsigned char)(p2 >> 8);
    data[8] = (unsigned char)p2;
    return si5351_write(device->conf.device_address, device->conf.channel, data, SI5351_PARAMETERS_LENGTH+1);
}

static int set_ms(si5351_dev *device, int output_no, unsigned int p1, unsigned int p2, unsigned int p3,
                    unsigned int div_by_4, unsigned int r_div)
{
    unsigned char data[SI5351_PARAMETERS_LENGTH+1];
    data[0] = (unsigned char)(SI5351_CLK0_PARAMETERS + output_no * SI5351_PARAMETERS_LENGTH);
    data[1] = (unsigned char)(p3 >> 8);
    data[2] = (unsigned char)p3;
    data[3] = (unsigned char)(((r_div << 4) & 0x70) | ((div_by_4 << 2) & 0x0C) | ((p1 >> 16) & 3));
    data[4] = (unsigned char)(p1 >> 8);
    data[5] = (unsigned char)p1;
    data[6] = (unsigned char)(((p3 >> 12) & 0xF0) | ((p2 >> 16) & 0x0F));
    data[7] = (unsigned char)(p2 >> 8);
    data[8] = (unsigned char)p2;
    return si5351_write(device->conf.device_address, device->conf.channel, data, SI5351_PARAMETERS_LENGTH+1);
}

static int set_pll(si5351_dev *device, enum si5351_pll pll, double coef)
{
    unsigned int a = (unsigned int)coef;
    double c = 1048575;
    unsigned int ic = 1048575;
    double b = (coef - a) * 1048575;
    unsigned int bc128 = (unsigned int)(b * 128 / c);
    unsigned int p1 = 128 * a + bc128 - 512;
    unsigned int p2 = 128 * (unsigned int)b - ic * bc128;
    return set_msn(device, pll, p1, p2, ic);
}

static int set_pll_parameters(si5351_dev *device)
{
    unsigned int coef = (PLL_MAX_FREQUENCY / device->conf.mClk) & 0xFFFFFFFE; // even number
    unsigned int pll_frequency = device->conf.mClk * coef;
    device->fVCO[0] = pll_frequency;
    device->fVCO[1] = pll_frequency;
    device->max_frequency = PLL_MAX_FREQUENCY / 4;
    device->max_frequency_plla = pll_frequency / 8;
    device->min_frequency = pll_frequency / 2048;
    int rc = set_pll(device, SI5351_PLLA, coef);
    if (rc)
        return rc;
    return set_pll(device, SI5351_PLLB, coef);
}

static int validate_output_no(si5351_dev *device, int output_no)
{
    return output_no < 0 || output_no >= device->conf.output_count;
}

static int update_clock_control(si5351_dev *device, int output_no)
{
    unsigned char data[2];
    data[0] = SI5351_CLK0_CTRL + (unsigned char)output_no;
    data[1] = device->clock_control[output_no];
    return si5351_write(device->conf.device_address, device->conf.channel, data, 2);
}

static int update_clock_enable(si5351_dev *device)
{
    unsigned char data[2];
    data[0] = SI5351_OUTPUT_ENABLE_CTRL;
    data[1] = device->clock_enable_flags;
    return si5351_write(device->conf.device_address, device->conf.channel, data, 2);
}

int si5351_init(const si5351_conf *conf, si5351_dev *device)
{
    memcpy((void*)&device->conf, conf, sizeof(si5351_conf));
    device->clock_enable_flags = 0xFF;
    // Select MultiSynth 0 as the source for CLKn.
    // CLKn is powered down.
    memset(device->clock_control, 0x8C, 8); // FBB_INT = 0
    device->clock_control[6] = 0xCC; // FBA_INT = 1

    for (int i = 0; i < 7; i++)
        update_clock_control(device, i);

    int rc = si5351_set_crystal_load(device, conf->load);
    if (rc)
        return rc;

    rc = update_clock_enable(device);
    if (rc)
        return rc;

    unsigned char data[2];
    data[0] = SI5351_SSC_PARAM0; // disable spread spectrum
    data[1] = 0;
    rc = si5351_write(device->conf.device_address, device->conf.channel, data, 2);
    if (rc)
        return rc;
    data[0] = SI5351_FANOUT_ENABLE; // disable fanout
    rc = si5351_write(device->conf.device_address, device->conf.channel, data, 2);
    if (rc)
        return rc;
    return set_pll_parameters(device);
}

int si5351_enable_output(si5351_dev *device, int output_no, int enable)
{
    int rc = validate_output_no(device, output_no);
    if (rc)
        return rc;
    if (enable)
    {
        device->clock_enable_flags &= ~(1 << output_no);
        device->clock_control[output_no] &= 0x7F;
    }
    else
    {
        device->clock_enable_flags |= 1 << output_no;
        device->clock_control[output_no] |= 0x80;
    }
    rc = update_clock_control(device, output_no);
    if (rc)
        return rc;
    return update_clock_enable(device);
}

static int set_hi_frequency(si5351_dev *device, int output_no, unsigned int frequency)
{
    if (frequency < PLL_MIN_FREQUENCY / 4) // cannot set frequency between 112.5 and 150 MHz
        return 10;
    double coef = (double)frequency * 4 / device->conf.mClk;
    int rc = set_pll(device, SI5351_PLLB, coef);
    if (rc)
        return rc;
    rc = set_ms(device, output_no, 0, 0, 1, 3, 0);
    if (rc)
        return rc;
    device->clock_control[output_no] |= 0x60; //MSx_INT=1,  MS0_SRC = PLLB
    return update_clock_control(device, output_no);
}

int si5351_set_frequency(si5351_dev *device, int output_no, unsigned int frequency, unsigned int divider)
{
    int rc = validate_output_no(device, output_no);
    if (rc)
        return rc;
    if (frequency < device->min_frequency || frequency > device->max_frequency)
        return 2;
    if (frequency > device->max_frequency_plla)
        return set_hi_frequency(device, output_no, frequency);
    unsigned int r_div;
    switch (divider)
    {
        case 1:
            r_div = 0;
            break;
        case 2:
            r_div = 1;
            break;
        case 4:
            r_div = 2;
            break;
        case 8:
            r_div = 3;
            break;
        case 16:
            r_div = 4;
            break;
        case 32:
            r_div = 5;
            break;
        case 64:
            r_div = 6;
            break;
        case 128:
            r_div = 7;
            break;
        default:
            return 3;
    }
    double coef = device->fVCO[0] / (double)frequency;
    unsigned int a = (unsigned int)coef;
    double c = 1048575;
    unsigned int ic = 1048575;
    double b = (coef - a) * 1048575;
    unsigned int bc128 = (unsigned int)(b * 128 / c);
    unsigned int p1 = 128 * a + bc128 - 512;
    unsigned int p2 = 128 * (unsigned int)b - ic * bc128;
    unsigned int div_by_4 = 0;
    rc = set_ms(device, output_no, p1, p2, ic, div_by_4, r_div);
    if (rc)
        return rc;
    device->clock_control[output_no] &= 0x9F; //MSx_INT=0,  MS0_SRC = PLLA
    return update_clock_control(device, output_no);
}

int si5351_set_drive_strength(si5351_dev *device, int output_no, enum si5351_drive drive)
{
    int rc = validate_output_no(device, output_no);
    if (rc)
        return rc;
    device->clock_control[output_no] &= 0xFC;
    switch (drive)
    {
        case SI5351_DRIVE_4MA:
            device->clock_control[output_no] |= 1;
            break;
        case SI5351_DRIVE_6MA:
            device->clock_control[output_no] |= 2;
            break;
        case SI5351_DRIVE_8MA:
            device->clock_control[output_no] |= 3;
            break;
        default:
            break;
    }
    return update_clock_control(device, output_no);
}

int si5351_get_status(const si5351_dev *device, unsigned char *status)
{
    return si5351_read(device->conf.device_address, device->conf.channel, SI5351_DEVICE_STATUS, status, 1);
}

int si5351_set_crystal_load(const si5351_dev *device, enum si5351_crystal_load load)
{
    unsigned char data[2];
    data[0] = SI5351_CRYSTAL_LOAD;
    data[1] = load;
    return si5351_write(device->conf.device_address, device->conf.channel, data, 2);
}
