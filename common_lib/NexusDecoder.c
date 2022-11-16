#include <NexusDecoder.h>

Queue nexusDecoderQueue;
static unsigned int lastTime;
static int bitNo;
static unsigned long long bits;

void NexusDecoderInit(int queue_size, char *buffer)
{
  queue_init(&nexusDecoderQueue, queue_size, sizeof(Nexus36Packet), buffer);
  lastTime = 0;
  bitNo = -1;
  bits = 0;
}

static int checkBits(void)
{
  Nexus36Packet p;

  if (bitNo == 36)
  {
    p.humidity = bits & 0xFF;
    //if (p.humidity > 100)
    //  return NDS_BAD_HUMIDITY;
    if ((bits & 0xF00) != 0xF00)
      return NDS_BAD_FLAG;
    bits >>= 12;
    p.temperature = bits & 0xFFF;
    if (p.temperature & 0x800)
      p.temperature |= 0xFFFFF000;
    if (p.temperature < -600 || p.temperature > 700)
      return NDS_BAD_TEMPERATURE;
    bits >>= 12;
    p.channel = bits & 3;
    bits >>= 3;
    p.low_battery = bits & 1;
    bits >>= 1;
    p.id = bits & 0xFF;
    queue_push(&nexusDecoderQueue, &p);
    bitNo = -1;
    return NDS_SUCCESS;
  }
  return NDS_NEXT_BIT;
}

int NexusDecoder(int edge, unsigned int time)
{
  unsigned int delta = time - lastTime;
  lastTime = time;
  if (bitNo < 0 && edge != 0 && delta >= 3900) // rising edge
  {
    bitNo = 0;
    return NDS_START;
  }
  if (bitNo >= 0)
  {
    if (edge == 0) // falling edge
    {
      if (delta < 400 || delta > 600)
      {
        bitNo = -1;
        return NDS_BAD_LEVEL_ONE_DURATION;
      }
      return NDS_FALLING_EDGE;
    }
    else // next bit
    {
      if (delta >= 900 && delta <= 1100) // zero bit
      {
        bitNo++;
        bits <<= 1;
        return checkBits();
      }
      if (delta >= 1900 && delta <= 2100) // one bit
      {
        bitNo++;
        bits <<= 1;
        bits |= 1;
        return checkBits();
      }
      bitNo = -1;
      return NDS_BAD_LEVEL_ZERO_DURATION;
    }
  }
  return NDS_WAIT;
}
