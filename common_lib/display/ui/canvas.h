#ifndef _CANVAS_H
#define _CANVAS_H

#include "gdevice.h"
#include <display.h>
#include <ts_point.h>

#define POS_ABSOLUTE 0
#define POS_NEXTH 1
#define POS_NEXTV 2
#define POS_RESTH 3
#define POS_RESTHM1 4
#define POS_RESTV 16

#define EV_CLICK 1

class API Canvas
{
  unsigned int bkColor, disabledBkColor, left, top, width, height, updated, isDisabled;
  Canvas *parent, *next, *firstChild, *lastChild;
  unsigned int clickEventId;

  void WriteTextData(GDevice* d, unsigned int x, unsigned int y, const unsigned char* bitmap, unsigned int height,
                     unsigned int bit, unsigned int char_bytes, unsigned int textColor, unsigned int bkColor);
  int isUpdated();
  unsigned int DrawChildren(GDevice* d);
  void AddChild(Canvas* child, unsigned int pos);
  void ResetUpdated();
  int isPointInside(ts_point* p);
protected:
  void SetUpdated();
  unsigned int DrawTextLeft(GDevice* d, unsigned int x, unsigned int y, const char* text, const FONT_INFO* f, unsigned int textColor);
  unsigned int DrawTextCenter(GDevice* d, unsigned int x, unsigned int y, const char* text, const FONT_INFO* f, unsigned int textColor, unsigned int width);
  unsigned int DrawTextRight(GDevice* d, unsigned int x, unsigned int y, const char* text, const FONT_INFO* f, unsigned int textColor, unsigned int width);
  void DrawRect(GDevice* d, unsigned int x, unsigned int y, unsigned int _width, unsigned int _height, unsigned int color);
  virtual void EraseBackground(GDevice* d);
  virtual void InternalDraw(GDevice* d);
  void SetWidth(unsigned int w);
  void SetHeight(unsigned int h);
  void SetBkColor(unsigned int color);
  void SetClickEventId(unsigned int id);
public:
  Canvas(Canvas *_parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor);
  int isInside(unsigned int x, unsigned int y);
  unsigned int GetBkColor();
  unsigned int GetWidth();
  unsigned int GetHeight();
  unsigned int GetTop();
  unsigned int GetLeft();
  virtual unsigned int Draw(GDevice *d);
  unsigned int Click(ts_point* p);
  void Disable();
  void Enable();
};

#endif // !_CANVAS_H
