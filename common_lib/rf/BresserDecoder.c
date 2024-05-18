#include <BresserDecoder.h>
#include <board.h>

/*

preamble: 750us "1" + 750 us "0" repeated 4 times
data length: 40 bits
0: 250 us "1" + 500 us "0"
1: 500 us "1" + 250 us "0"

Example:

0000 id = 9
1001

0001 bat_ok, ch = 1

0110 temp = 65A (1626) = 1626-900 *0.1 = 72.6 F = 22,55 C
0101
1010

0010 humidity = 2C (44)
1100

1010 checksum = A5 = 09 + 16 + 5A + 2C
0101
*/

Queue bresserDecoderQueue;
static unsigned int lastTime;
static int bitNo, sync, shouldBe1;
static union {
  long long bits;
  unsigned char bytes[8];
  short words[4];
} data;

void BresserDecoderInit(int queue_size, char *buffer)
{
  queue_init(&bresserDecoderQueue, queue_size, sizeof(BresserPacket), buffer);
  lastTime = 0;
  bitNo = sync = -1;
  data.bits = 0;
}

void BresserDecoderReset(void)
{
  queue_reset(&bresserDecoderQueue);
  lastTime = 0;
  bitNo = sync = -1;
  data.bits = 0;
}

static int checkBits(void)
{
  BresserPacket p;
  unsigned char sum;
  if (bitNo == 40)
  {
    bitNo = -1;
    sum = data.bytes[4] + data.bytes[3] + data.bytes[2] + data.bytes[1];
    if (sum != data.bytes[0])
      return BDS_BAD_CRC;
    p.humidity = data.bytes[1];
    if (p.humidity > 100)
      return BDS_BAD_HUMIDITY;
    p.id = data.bytes[4];
    sum = data.bytes[3] >> 4;
    p.low_battery = sum & 0x8;
    p.channel = sum & 7;
    // C = (F - 32) / 1.8
    p.temperature = ((data.words[1] & 0xFFF) - 900 - 320) * 10 / 18;
    queue_push(&bresserDecoderQueue, &p);
    return BDS_SUCCESS;
  }
  return BDS_NEXT_BIT;
}

int BresserDecoder(int edge, unsigned int time)
{
  unsigned int delta = time - lastTime;
  lastTime = time;
  if (sync < 0 && bitNo < 0 && edge != 0) // rising edge
  {
    sync = 0;
    return BDS_CHECKSYNC;
  }
  if (sync >= 0)
  {
    if (delta >= 700 && delta <= 900)
    {
      sync++;
      if (sync == 8)
      {
        bitNo = 0;
        sync = -1;
        return BDS_SYNC_OK;
      }
    }
    else
    {
      sync = -1;
      return BDS_BADSYNC;
    }
  }
  if (bitNo >= 0)
  {
    if (edge == 0) // falling edge
    {
      if (delta < 200 || delta > 600)
      {
        bitNo = -1;
        return BDS_BAD_LEVEL_ONE_DURATION;
      }
      shouldBe1 = delta >= 400;
      return BDS_FALLING_EDGE;
    }
    else // next bit
    {
      if (shouldBe1)
      {
        if (delta < 200 || delta > 300)
        {
          bitNo = -1;
          return BDS_BAD_LEVEL_ZERO_DURATION;
        }
        data.bits <<= 1;
        data.bits |= 1;
      }
      else
      {
        if (delta < 400 || delta > 600)
        {
          bitNo = -1;
          return BDS_BAD_LEVEL_ZERO_DURATION;
        }
        data.bits <<= 1;
      }
      bitNo++;
      return checkBits();
    }
  }
  return BDS_WAIT;
}
