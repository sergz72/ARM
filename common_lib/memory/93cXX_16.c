#include <93cXX_16.h>
#include "board.h"

static int wait_not_busy(int channel, int timeout)
{
  _93CXX_DI_LOW(channel);
  _93CXX_CLK_LOW(channel);
  _93CXX_CS_HIGH(channel);
  while (timeout--)
  {
    _93CXX_DELAY;
    if (_93CXX_DO(channel))
    {
      _93CXX_CS_LOW(channel);
      _93CXX_DELAY;
      return 0;
    }
  }
  _93CXX_CS_LOW(channel);
  return 1;
}

static unsigned short send(int channel, unsigned int data, int i)
{
  unsigned short result = 0;
  const unsigned int mask = (1 << i);
  _93CXX_DI_HIGH(channel);
  _93CXX_CLK_LOW(channel);
  _93CXX_CS_HIGH(channel);
  _93CXX_DELAY;
  _93CXX_CLK_HIGH(channel);
  _93CXX_DELAY;
  _93CXX_DELAY;
  _93CXX_CLK_LOW(channel);
  for (; i >= 0; i--)
  {
    _93CXX_DELAY;
    if (data & mask)
      _93CXX_DI_HIGH(channel);
    else
      _93CXX_DI_LOW(channel);
    _93CXX_DELAY;
    _93CXX_CLK_HIGH(channel);
    _93CXX_DELAY;
    _93CXX_DELAY;
    if (i < 16)
    {
      result <<= 1;
      if (_93CXX_DO(channel))
        result |= 1;
    }
    _93CXX_CLK_LOW(channel);
    data <<= 1;
  }
  _93CXX_DELAY;
  _93CXX_CS_LOW(channel);
  return result;
}

void _93CXX_16_write_enable(int channel)
{
  send(channel, 0xFF, 9);
}

int _93CXX_16_erase(int channel, unsigned int address, int timeout)
{
  address &= 0xFF;
  send(channel, 0x300 | address, 9);
  return wait_not_busy(channel, timeout);
}

int _93CXX_16_write(int channel, unsigned int address, unsigned int data, int timeout)
{
  address &= 0xFF;
  data &= 0xFFFF;
  send(channel, (1 << 24) | (address << 16) | data, 25);
  return wait_not_busy(channel, timeout);
}

void _93CXX_16_read(int channel, unsigned int address, unsigned short *data)
{
  address &= 0xFF;
  *data = send(channel, (2 << 24) | (address << 16), 25);
}

