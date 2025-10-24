#ifndef _ADS1220_EMULATOR_H
#define _ADS1220_EMULATOR_H

#include "board.h"

#define ADS1220_INPUT_CHANNELS 4

typedef struct
{
  int vdda_mv;
  int vref_mv;
  int ain_uv[ADS1220_INPUT_CHANNELS];
  int temperature;
} ads1220_emulator_configuration;

extern ads1220_emulator_configuration ads1220_emulator_config[AD7793_MAX_CHANNELS];

#ifdef __cplusplus
extern "C" {
#endif

void ads1220_emulator_init(void);

#ifdef __cplusplus
}
#endif

#endif
