#ifndef _MULTIMETER_INIT_H
#define _MULTIMETER_INIT_H

#include "multimeter.h"

#define ALWAYS_ENABLED_MEASUREMENTS ((1<<CHANNEL_TYPE_VOLTAGE)|(1<<CHANNEL_TYPE_CURRENT)|\
                                     (1<<CHANNEL_TYPE_VDDA)|(1<<CHANNEL_TYPE_TEMPERATURE))

#define DEFAULT_ENABLED_MEASUREMENTS (ALWAYS_ENABLED_MEASUREMENTS|(1<<CHANNEL_TYPE_FREQUENCY))

extern Multimeter multimeter;

void multimeter_init();

#endif