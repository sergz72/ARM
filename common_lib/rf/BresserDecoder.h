#ifndef _BRESSER_DECODER_H
#define _BRESSER_DECODER_H

#define BDS_SUCCESS  0
#define BDS_WAIT  1
#define BDS_START 2
#define BDS_BAD_LEVEL_ONE_DURATION 3
#define BDS_BAD_LEVEL_ZERO_DURATION 4
#define BDS_FALLING_EDGE 5
#define BDS_CHECKSYNC 6
#define BDS_BADSYNC 7
#define BDS_SYNC_OK 8
#define BDS_BAD_CRC 9
#define BDS_BAD_HUMIDITY 10
#define BDS_NEXT_BIT 11

#include <fixed_queue.h>

typedef struct {
  unsigned int id, low_battery, channel, humidity;
  int temperature;
} BresserPacket;

void BresserDecoderInit(int queue_size, char* buffer);
int BresserDecoder(int edge, unsigned int time);
void BresserDecoderReset(void);

extern Queue bresserDecoderQueue;

#endif
