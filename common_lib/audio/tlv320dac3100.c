#include <audio.h>
#include <tlv320dac3100.h>

static unsigned char current_page;

int tlv320dac3100_init(void)
{
  current_page = 0;
  return 0;
}

int tlv320dac3100_set_page(unsigned char page)
{
  if (page != current_page)
  {
    current_page = page;
    return tlv320dac3100_write(TLV320DAC3100_REGISTER_PAGE, page);
  }
  return 0;
}

int tlv320dac3100_set_interface(const TLV320DAC3100_InterfaceControl *control)
{
  int rc = tlv320dac3100_set_page(0);
  if (rc)
    return rc;
  unsigned char value = control->interface_type | control->word_length;
  if (control->bclk_output)
    value |= 8;
  if (control->wclk_output)
    value |= 4;
  rc = tlv320dac3100_write(TLV320DAC3100_REGISTER_INTERFACE_CONTROL1, value);
  if (rc)
    return rc;
  value = control->bclk_inverted ? 8 : 0;
  if (control->bclk_wclk_active_in_power_down)
    value |= 4;
  if (!control->bdiv_clkin_eq_dac_clk)
    value |= 1;
  return tlv320dac3100_write(TLV320DAC3100_REGISTER_INTERFACE_CONTROL2, value);
}

int tlv320dac3100_set_clock(unsigned char pll_clkin, unsigned char codec_clkin)
{
  int rc = tlv320dac3100_set_page(0);
  if (rc)
    return rc;
  return tlv320dac3100_write(TLV320DAC3100_REGISTER_CLOCK_MUX, pll_clkin | codec_clkin);
}

int tlv320dac3100_set_pll(int enable, unsigned char p, unsigned char r, unsigned char j, unsigned short d)
{
  // Validate all input ranges
  if (p < 1 || p > 8) {
    return 1;
  }
  if (r < 1 || r > 16) {
    return 2;
  }
  if (j < 1 || j > 63) {
    return 3;
  }
  if (d > 9999) {
    return 4;
  }
  int rc = tlv320dac3100_set_page(0);
  if (rc)
    return rc;
  unsigned char pr = p == 8 ? 0 : (p << 4);
  pr |= r = 16 ? 0 : r;
  if (enable)
    pr |= 0x80;
  rc = tlv320dac3100_write(TLV320DAC3100_REGISTER_PLL_J, j);
  if (rc)
    return rc;
  rc = tlv320dac3100_write(TLV320DAC3100_REGISTER_PLL_D_MSB, (unsigned char)(d >> 8));
  if (rc)
    return rc;
  rc = tlv320dac3100_write(TLV320DAC3100_REGISTER_PLL_D_LSB, (unsigned char)d);
  if (rc)
    return rc;
  return tlv320dac3100_write(TLV320DAC3100_REGISTER_PLL_PR, pr);
}

static int tlv320dac3100_set_div7(unsigned char reg, int enable, unsigned char divider)
{
  if (divider == 0 || divider > 128)
    return 1;
  if (divider == 128)
    divider = 0;
  if (enable)
    divider |= 0x80;
  int rc = tlv320dac3100_set_page(0);
  if (rc)
    return rc;
  return tlv320dac3100_write(reg, divider);
}

int tlv320dac3100_set_ndac(int enable, unsigned char divider)
{
  return tlv320dac3100_set_div7(TLV320DAC3100_REGISTER_NDAC, enable, divider);
}

int tlv320dac3100_set_mdac(int enable, unsigned char divider)
{
  return tlv320dac3100_set_div7(TLV320DAC3100_REGISTER_MDAC, enable, divider);
}

int tlv320dac3100_set_dosr(unsigned short value)
{
  if (value < 2 || value == 1023 || value > 1024)
    return 1;
  if (value == 1024)
    value = 0;
  int rc = tlv320dac3100_set_page(0);
  if (rc)
    return rc;
  rc = tlv320dac3100_write(TLV320DAC3100_REGISTER_DOSR_MSB, (unsigned char)(value >> 8));
  if (rc)
    return rc;
  return tlv320dac3100_write(TLV320DAC3100_REGISTER_DOSR_LSB, (unsigned char)value);
}

int tlv320dac3100_set_datapath(const TLV320DAC3100_DataPath *path)
{
  int rc = tlv320dac3100_set_page(0);
  if (rc)
    return rc;
  unsigned char value = path->soft_stepping | (path->data_path[CHANNEL_LEFT] << 4) | (path->data_path[CHANNEL_RIGHT] << 2);
  if (path->channel_power_up[CHANNEL_LEFT])
    value |= 0x80;
  if (path->channel_power_up[CHANNEL_RIGHT])
    value |= 0x40;
  return tlv320dac3100_write(TLV320DAC3100_REGISTER_DAC_DATA_PATH, value);
}

int tlv320dac3100_set_routing(const TLV320DAC3100_Routing *routing)
{
  int rc = tlv320dac3100_set_page(1);
  if (rc)
    return rc;
  unsigned char value = (routing->dac_routing[CHANNEL_LEFT] << 6) | (routing->dac_routing[CHANNEL_RIGHT] << 2);
  if (routing->ain1_left)
    value |= 0x20;
  if (routing->ain2_left)
    value |= 0x10;
  if (routing->ain2_right)
    value |= 2;
  if (routing->differential_output_mode)
    value |= 1;
  return tlv320dac3100_write(TLV320DAC3100_REGISTER_MIXER_ROUTING, value);
}

int tlv320dac3100_dac_volume_control(int mute_left, int mute_right, unsigned char volume_control)
{
  int rc = tlv320dac3100_set_page(0);
  if (rc)
    return rc;
  unsigned char value = volume_control;
  if (mute_left)
    value |= 8;
  if (mute_right)
    value |= 4;
  return tlv320dac3100_write(TLV320DAC3100_REGISTER_DAC_VOLUME, value);
}

int tlv320dac3100_dac_volume(int channel, int dbx10)
{
  if (dbx10 > 240)
    dbx10 = 240;
  else if (dbx10 < -635)
    dbx10 = -635;
  dbx10 /= 5;
  return tlv320dac3100_write(channel == CHANNEL_LEFT ?
    TLV320DAC3100_REGISTER_DAC_LEFT_VOLUME : TLV320DAC3100_REGISTER_DAC_RIGHT_VOLUME, (unsigned char)dbx10);
}

int tlv320dac3100_set_headphone_driver(const TLV320DAC3100_HPControl *control)
{
  int rc = tlv320dac3100_set_page(1);
  if (rc)
    return rc;
  unsigned char value = control->common_mode_voltage;
  if (control->driver_power_up[CHANNEL_LEFT])
    value |= 0x80;
  if (control->driver_power_up[CHANNEL_RIGHT])
    value |= 0x40;
  if (control->short_circuit_protection_enable)
    value |= 2;
  return tlv320dac3100_write(TLV320DAC3100_REGISTER_HEADPHONE_DRIVERS, value);
}

int tlv320dac3100_set_headphone_volume(int channel, int enable, unsigned char value)
{
  if (value > 0x7F)
    return 1;
  if (enable)
    value |= 0x80;
  int rc = tlv320dac3100_set_page(1);
  if (rc)
    return rc;
  return tlv320dac3100_write(channel == CHANNEL_LEFT ?
    TLV320DAC3100_REGISTER_ANALOG_VOLUME_HPL : TLV320DAC3100_REGISTER_ANALOG_VOLUME_HPR, value);
}

int tlv320dac3100_set_headphone_pga(int channel, int unmute, unsigned char gain)
{
  if (gain > 9)
    return 1;
  unsigned char value = gain << 3;
  value |= 2;
  if (unmute)
    value |= 4;
  int rc = tlv320dac3100_set_page(1);
  if (rc)
    return rc;
  return tlv320dac3100_write(channel == CHANNEL_LEFT ?
    TLV320DAC3100_REGISTER_HPL_DRIVER : TLV320DAC3100_REGISTER_HPR_DRIVER, value);
}

int tlv320dac3100_set_clkout(int enable, unsigned char source, unsigned char divider)
{
  if (source > 5)
    return 1;
  if (divider > 128)
    return 2;
  if (divider == 128)
    divider = 0;
  if (enable)
    divider |= 0x80;
  int rc = tlv320dac3100_set_page(0);
  if (rc)
    return rc;
  rc = tlv320dac3100_write(TLV320DAC3100_REGISTER_CLKOUT_MUX, source);
  if (rc)
    return rc;
  rc = tlv320dac3100_write(TLV320DAC3100_REGISTER_CLKOUT_M, divider);
  if (rc)
    return rc;
  return tlv320dac3100_write(TLV320DAC3100_REGISTER_GPIO1_CONTROL, enable ? TLV320DAC3100_GPIO1_CLKOUT : TLV320DAC3100_GPIO1_DISABLED);
}
