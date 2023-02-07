#ifndef _NEXUS_DECODER_H
#define _NEXUS_DECODER_H

#define NDS_SUCCESS  0
#define NDS_WAIT  1
#define NDS_START 2
#define NDS_BAD_LEVEL_ONE_DURATION 3
#define NDS_BAD_LEVEL_ZERO_DURATION 4
#define NDS_FALLING_EDGE 5
#define NDS_NEXT_BIT 6
#define NDS_BAD_HUMIDITY 7
#define NDS_BAD_FLAG 8
#define NDS_BAD_TEMPERATURE 9

#include <fixed_queue.h>

typedef struct {
  unsigned int id, low_battery, channel, humidity;
  int temperature;
} Nexus36Packet;

void NexusDecoderInit(int queue_size, char* buffer);
int NexusDecoder(int edge, unsigned int time);

extern Queue nexusDecoderQueue;

#endif
