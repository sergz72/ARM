#include <ad9910.h>
#include "board.h"
#include <string.h>

#define NUM_REGISTERS               22
#define CFR1_REGISTER               0
#define CFR2_REGISTER               1
#define CFR3_REGISTER               2
#define AUX_DAC_REGISTER            3
#define IO_UPDATE_REGISTER          4
#define FTW_REGISTER                7
#define POW_REGISTER                8
#define ASF_REGISTER                9
#define MULTICHIP_SYNC_REGISTER     10
#define DIGITAL_RAMP_LIMIT_REGISTER 11
#define DIGITAL_RAMP_STEP_REGISTER  12
#define DIGITAL_RAMP_RATE_REGISTER  13
#define PROFILE0_REGISTER           14
#define PROFILE1_REGISTER           15
#define PROFILE2_REGISTER           16
#define PROFILE3_REGISTER           17
#define PROFILE4_REGISTER           18
#define PROFILE5_REGISTER           19
#define PROFILE6_REGISTER           20
#define PROFILE7_REGISTER           21
#define RAM_REGISTER                22

static void ad9910_write32(int channel, unsigned char address, unsigned int data)
{
  unsigned char cdata[4];
  cdata[0] = (unsigned char)(data >> 24);
  cdata[1] = (unsigned char)((data >> 16) & 0xFF);
  cdata[2] = (unsigned char)((data >> 8) & 0xFF);
  cdata[3] = (unsigned char)(data & 0xFF);
  ad9910_write(channel, address, cdata, 4);
  AD9910_IO_UPDATE_SET(channel);
  delay_us(1);
  AD9910_IO_UPDATE_CLR(channel);
}

static void ad9910_write64(int channel, unsigned char address, unsigned int data_hi, unsigned int data_lo)
{
  unsigned char cdata[8];
  cdata[0] = (unsigned char)(data_hi >> 24);
  cdata[1] = (unsigned char)((data_hi >> 16) & 0xFF);
  cdata[2] = (unsigned char)((data_hi >> 8) & 0xFF);
  cdata[3] = (unsigned char)(data_hi & 0xFF);
  cdata[4] = (unsigned char)(data_lo >> 24);
  cdata[5] = (unsigned char)((data_lo >> 16) & 0xFF);
  cdata[6] = (unsigned char)((data_lo >> 8) & 0xFF);
  cdata[7] = (unsigned char)(data_lo & 0xFF);
  ad9910_write(channel, address, cdata, 8);
  AD9910_IO_UPDATE_SET(channel);
  delay_us(1);
  AD9910_IO_UPDATE_CLR(channel);
}

static int configure_pll(int channel, const AD9910_Config *config)
{
  if (config->pllclk % config->refclk)
    return 1;
  unsigned int n = config->pllclk / config->refclk;
  if (!n)
    return 2;
  if (n >= 128)
    return 3;
  unsigned int vco = 0;
  if (config->pllclk < 400000000 || config->pllclk > 1030000000)
    return 4;
  if (config->pllclk > 460000000)
  {
    if (config->pllclk < 530000000)
      vco = 1 << 24;
    else if (config->pllclk < 615000000)
      vco = 2 << 24;
    else if (config->pllclk < 650000000)
      return 5;
    else if (config->pllclk < 790000000)
      vco = 3 << 24;
    else if (config->pllclk < 875000000)
      vco = 4 << 24;
    else if (config->pllclk < 920000000)
      return 6;
    else
      vco = 5 << 24;
  }
  unsigned int icp = config->charge_pump_current_ua < 237 ? 0 : (config->charge_pump_current_ua - 212) / 15;
  if (icp > 7)
    icp = 7;
  ad9910_write32(channel, CFR3_REGISTER, vco | (icp << 19) | (1 << 14) | (1 << 8) | (n << 1));
  delay_ms(10);
  return AD9910_PLL_LOCK_CHECK(channel) == 0 ? 7 : 0;
}

static void ad9910_reset(int channel)
{
  AD9910_RESET_SET(channel);
  delay_us(1);
  AD9910_RESET_CLR(channel);
}

int ad9910_init(int channel, const AD9910_Config *config)
{
  ad9910_reset(channel);
  ad9910_power_up(channel);
  int rc = configure_pll(channel, config);
  if (!rc)
    ad9910_write32(channel, CFR2_REGISTER, 0x01000020); // enable amplitude scale from single tone profiles
  ad9910_power_down(channel);
  return rc;
}

void ad9910_power_up(int channel)
{
  AD9910_EXT_PWR_DOWN_CLR(channel);
  delay_ms(1);
}

void ad9910_power_down(int channel)
{
  AD9910_EXT_PWR_DOWN_SET(channel);
}

void ad9910_set_freq_phase_amplitude(int channel, unsigned long freq_word, unsigned int phase, unsigned int amplitude)
{
  ad9910_write64(channel, PROFILE0_REGISTER, phase | (amplitude << 16), freq_word);
}
