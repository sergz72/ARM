#ifndef _PROLOGUE_DECODER_H
#define _PROLOGUE_DECODER_H

#define PDS_SUCCESS  0
#define PDS_WAIT  1
#define PDS_START 2
#define PDS_BAD_LEVEL_ONE_DURATION 3
#define PDS_BAD_LEVEL_ZERO_DURATION 4
#define PDS_FALLING_EDGE 5
#define PDS_NEXT_BIT 6
#define PDS_BAD_HUMIDITY 7
#define PDS_BAD_TYPE 8
#define PDS_BAD_TEMPERATURE 9

#include <fixed_queue.h>

typedef struct {
  unsigned int type, id, low_battery, channel, humidity;
  int temperature;
} ProloguePacket;

void PrologueDecoderInit(int queue_size, char* buffer);
int PrologueDecoder(int edge, unsigned int time);

extern Queue prologueDecoderQueue;

#endif
