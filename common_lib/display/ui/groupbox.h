#ifndef _GROUPBOX_H
#define _GROUPBOX_H

#include "canvas.h"

class API GroupBox : public Canvas
{
  const char* text;
  unsigned int textColor, frameColor;
  const FONT_INFO* font;
protected:
  virtual void InternalDraw(GDevice* d);
public:
  GroupBox(Canvas* _parent, unsigned int pos,  unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor,
           const char* _text, const FONT_INFO* _font, unsigned int _frameColor);
  GroupBox(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _textColor,
           const char* _text, const FONT_INFO* _font, unsigned int _frameColor);
  void SetText(const char* _text);
};
#endif

