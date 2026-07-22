#include "board.h"
#include "rf_modem_func.h"
#include <cc1101_func.h>
#include "config.h"
#include "env_func.h"

int rf_modem_init(void)
{
  switch (device_configuration.rf_modem_type)
  {
    case RF_MODEM_TYPE_CC1101:
      return cc1101Init(device_configuration.device_address, sensor_data_length, device_configuration.frequency,
                        device_configuration.rf_mode, device_configuration.tx_power);
    default:
      return 1;
  }
}

void send_env()
{

}
