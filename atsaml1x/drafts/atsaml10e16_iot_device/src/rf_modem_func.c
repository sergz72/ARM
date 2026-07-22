#include "board.h"
#include "rf_modem_func.h"
#include <cc1101_func.h>
#include "config.h"
#include "env_func.h"
#include <trng.h>

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

int send_env(void)
{
  int rc;
  unsigned int random[SEND_REPEATS];

  trng_generate(random, SEND_REPEATS);

  int i = 0;
  for (;;)
  {
    switch (device_configuration.rf_modem_type)
    {
      case RF_MODEM_TYPE_CC1101:
        rc = cc1101Send(0, (unsigned char*)&sensor_data, sensor_data_length);
        break;
      default:
        return 1;
    }
    if (rc || i == SEND_REPEATS - 1)
      return rc;
    i++;
    unsigned int delay = SEND_REPEAT_INTERVAL_MIN + (random[i] % (SEND_REPEAT_INTERVAL_MAX - SEND_REPEAT_INTERVAL_MIN));
    delayms(delay);
  }
  return 0;
}
