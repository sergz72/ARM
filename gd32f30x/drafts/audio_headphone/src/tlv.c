#include "tlv.h"
#include <tlv320dac3100.h>
#include "audio.h"
#include "board.h"

static const TLV320DAC3100_DataPath data_path = {
  .channel_power_up = {1,1},
  .data_path = {TLV320DAC3100_DATA_PATH_NORMAL, TLV320DAC3100_DATA_PATH_NORMAL},
  .soft_stepping = TLV320DAC3100_SOFT_STEPPING_ONE_STEP_PER_SAMPLE
};

static const TLV320DAC3100_Routing routing = {
  .ain1_left = 0,
  .ain2_left = 0,
  .ain2_right = 0,
  .differential_output_mode = 0,
  .dac_routing = {TLV320DAC3100_DAC_ROUTING_MIXER, TLV320DAC3100_DAC_ROUTING_MIXER}
};

static const TLV320DAC3100_HPControl hp_control = {
  .common_mode_voltage = TLV320DAC3100_HP_COMMON_MODE_VOLTAGE_1800,
  .driver_power_up = {1,1},
  .short_circuit_protection_enable = 1
};

int tlv_init(void)
{
  delayms(10);
  TLV_RESET_RELEASE;
  delayms(10);
  int rc = tlv320dac3100_init();
  if (rc)
    return rc;
  rc = tlv320dac3100_set_clock(TLV320DAC3100_PLL_CLKIN_BCLK, TLV320DAC3100_CODEC_CLKIN_PLL);
  if (rc)
    return rc;
  rc = tlv320dac3100_set_ndac(1, 20);
  if (rc)
    return rc;
  rc = tlv320dac3100_set_mdac(1, 2);
  if (rc)
    return rc;
  rc = tlv320dac3100_set_dosr(128);
  if (rc)
    return rc;
  //pll_clk = 512KHz * 160
  rc = tlv320dac3100_set_pll(1, 1, 16, 10, 0);
  if (rc)
    return rc;
  rc = tlv320dac3100_set_datapath(&data_path);
  if (rc)
    return rc;
  rc = tlv320dac3100_set_routing(&routing);
  if (rc)
    return rc;
  rc = tlv320dac3100_dac_volume_control(0, 0, TLV320DAC3100_VOLUME_CONTROL_INDEPENDENT);
  if (rc)
    return rc;
  rc = tlv320dac3100_set_headphone_volume(CHANNEL_LEFT, 1, 60); // -20 db
  if (rc)
    return rc;
  rc = tlv320dac3100_set_headphone_volume(CHANNEL_RIGHT, 1, 60); // -20 db
  if (rc)
    return rc;
  rc = tlv320dac3100_set_headphone_pga(CHANNEL_LEFT, 1, 0);
  if (rc)
    return rc;
  rc = tlv320dac3100_set_headphone_pga(CHANNEL_RIGHT, 1, 0);
  if (rc)
    return rc;
  return tlv320dac3100_set_headphone_driver(&hp_control);
}
