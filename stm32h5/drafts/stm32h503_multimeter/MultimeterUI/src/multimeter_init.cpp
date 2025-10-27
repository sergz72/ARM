#include "multimeter_init.h"
#include "MeasurementUnitAD7793.h"
#include "MeasurementUnitADS1220.h"
#include "MeasurementUnitInternal.h"
#include <ad7793.h>
#include <ads1220.h>

static MeasurementUnitAD7793 UnitAD7793(AD7793_SET_CHANNEL_CURRENT_SOURCE_CALLBACK);
static MeasurementUnitADS1220 UnitADS1220(ADS1220_SET_CHANNEL_CURRENT_SOURCE_CALLBACK);
static MeasurementUnitInternal UnitInternal;

static const MeasurementUint *measurement_units[MEASUREMENT_UNITS_COUNT] = {
  &UnitAD7793, &UnitADS1220, &UnitInternal
};

static const ad7793_configuration ad7793_config =
{
  .vbias = AD7793_VBIAS_AIN2M,
  .gain = AD7793_GAIN_1,
  .channel = AD7793_CHANNEL_AIN1N_AIN1N,
  .burnout_enable = 0,
  .unipolar = 1,
  .boost = 1,
  .internal_reference = 1,
  .buffer_enable = 1
};

static const ads1220_configuration ads1220_config =
{
  .mux = ADS1220_MUX_AIN0_AIN1,
  .gain = ADS1220_PGA_GAIN_1,
  .data_rate = ADS1220_DATA_RATE_20,
  .mode = ADS1220_MODE_NORMAL,
  .vref = ADS1220_VREF_INTERNAL,
  .filter_50_60 = ADS1220_FILTER_50,
  .idac_current = ADS1220_IDAC_10,
  .idac1_mux = ADS1220_IDAC_MUX_REFN0,
  .idac2_mux = ADS1220_IDAC_MUX_OFF,
  .data_ready_on_dout = 0,
  .auto_close_low_side_switch = 0,
  .continuous_conversion_mode = 0,
  .temperature_sensor_mode = 0,
  .burnout_sources_enable = 0,
  .pga_bypass = 0,
};

Multimeter multimeter(const_cast<MeasurementUint**>(measurement_units), DEFAULT_ENABLED_MEASUREMENTS, 100);

void multimeter_init()
{
  ad7793_init();
  ads1220_init();

  ad7793_set_mode_and_update_rate(0, AD7793_MODE_IDLE, AD7793_UPDATE_RATE_16_80);
  ad7793_set_configuration(0, &ad7793_config);
  ad7793_set_io(0, AD7793_IEXCDIR_IEXC1_IOUT1_IEXC2_IOUT2, AD7793_IEXCEN_10);

  ads1220_set_configuration(0, &ads1220_config);
}
