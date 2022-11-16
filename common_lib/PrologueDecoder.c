#include <PrologueDecoder.h>

Queue prologueDecoderQueue;
static unsigned int lastTime;
static int bitNo;
static unsigned long long bits;

void PrologueDecoderInit(int queue_size, char *buffer)
{
  queue_init(&prologueDecoderQueue, queue_size, sizeof(ProloguePacket), buffer);
  lastTime = 0;
  bitNo = -1;
  bits = 0;
}

static int checkBits(void)
{
  ProloguePacket p;

  if (bitNo == 36)
  {
    p.humidity = bits & 0xFF;
    //if (p.humidity > 100)
    //  return PDS_BAD_HUMIDITY;
    bits >>= 8;
    p.temperature = bits & 0xFFF;
    if (p.temperature & 0x800)
      p.temperature |= 0xFFFFF000;
    if (p.temperature < -600 || p.temperature > 700)
      return PDS_BAD_TEMPERATURE;
    bits >>= 12;
    p.channel = bits & 3;
    bits >>= 3;
    p.low_battery = bits & 1;
    bits >>= 1;
    p.id = bits & 0xFF;
    bits >>= 8;
    p.type = bits & 0x0F;
    if (p.type != 5 && p.type != 9)
      return PDS_BAD_TYPE;
    queue_push(&prologueDecoderQueue, &p);
    bitNo = -1;
    return PDS_SUCCESS;
  }
  return PDS_NEXT_BIT;
}

int PrologueDecoder(int edge, unsigned int time)
{
  unsigned int delta = time - lastTime;
  lastTime = time;
  if (bitNo < 0 && edge != 0 && delta >= 8900) // rising edge
  {
    bitNo = 0;
    return PDS_START;
  }
  if (bitNo >= 0)
  {
    if (edge == 0) // falling edge
    {
      if (delta < 400 || delta > 600)
      {
        bitNo = -1;
        return PDS_BAD_LEVEL_ONE_DURATION;
      }
      return PDS_FALLING_EDGE;
    }
    else // next bit
    {
      if (delta >= 1900 && delta <= 2100) // zero bit
      {
        bitNo++;
        bits <<= 1;
        return checkBits();
      }
      if (delta >= 3900 && delta <= 4100) // one bit
      {
        bitNo++;
        bits <<= 1;
        bits |= 1;
        return checkBits();
      }
      bitNo = -1;
      return PDS_BAD_LEVEL_ZERO_DURATION;
    }
  }
  return PDS_WAIT;
}
