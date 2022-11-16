#ifndef _GDEVICE_H
#define _GDEVICE_H

#include <font.h>
#include <display.h>

class API GDevice
{
  unsigned int width, height;
  FONT_CHAR_INFO *fci;
public:
  GDevice(unsigned int _width, unsigned int _height, FONT_CHAR_INFO *_fci);
  unsigned int GetWidth();
  unsigned int GetHeight();
  FONT_CHAR_INFO* GetFCI();
  virtual unsigned int RGBToDevice(unsigned int R, unsigned int G, unsigned int B) = 0;
  virtual void DrawPixel(unsigned int x, unsigned int y, unsigned int color) = 0;
  virtual void FillRect(unsigned int x, unsigned int y, unsigned int rwidth, unsigned int rheight, unsigned int color) = 0;
  virtual void DrawRect(unsigned int x, unsigned int y, unsigned int rwidth, unsigned int rheight, unsigned int color) = 0;
  virtual void Update() = 0;
};

#endif
