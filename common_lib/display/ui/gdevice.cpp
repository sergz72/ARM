#define PROJECT_EXPORTS
#include "gdevice.h"

GDevice::GDevice(unsigned int _width, unsigned int _height, FONT_CHAR_INFO *_fci)
{
  width = _width;
  height = _height;
  fci = _fci;
}

unsigned int GDevice::GetWidth()
{
  return width;
}

unsigned int GDevice::GetHeight()
{
  return height;
}

FONT_CHAR_INFO* GDevice::GetFCI()
{
  return fci;
}
