#ifndef _LABEL_H
#define _LABEL_H

#include <font.h>
#include "canvas.h"

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

class API Label : public Canvas
{
  const char* text;
  unsigned int textColor;
  unsigned int textAlign;
  const FONT_INFO* font;
protected:
  void CalculateWidth();
  virtual void InternalDraw(GDevice* d);
public:
  Label(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor,
        const char *_text, const FONT_INFO* _font, unsigned int _textAlign);
  Label(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _textColor,
        const char* _text, const FONT_INFO* _font, unsigned int _textAlign);
  Label(Canvas* _parent, unsigned int pos, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor, const char* _text, const FONT_INFO* _font, unsigned int _textAlign);
  Label(Canvas* _parent, unsigned int pos, unsigned int _textColor, const char* _text, const FONT_INFO* _font, unsigned int _textAlign);
  void SetText(const char* _text);
};

#endif
