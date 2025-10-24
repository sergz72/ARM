#ifndef _AD7793_EMULATOR_H
#define _AD7793_EMULATOR_H

#include "board.h"

#define AD7793_INPUT_CHANNELS 3

typedef struct
{
  int vdda_mv;
  int vref_mv;
  int ain_uv[AD7793_INPUT_CHANNELS];
} ad7793_emulator_configuration;

extern ad7793_emulator_configuration ad7793_emulator_config[AD7793_MAX_CHANNELS];

#ifdef __cplusplus
extern "C" {
#endif

void ad7793_emulator_init(void);

#ifdef __cplusplus
}
#endif

#endif