#ifndef _BUTTON_H
#define _BUTTON_H

#include "label.h"

class API Button : public Label
{
public:
  Button(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor,
         const char* _text, const FONT_INFO* _font, unsigned int _clickEventId);
  Button(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _textColor,
         const char* _text, const FONT_INFO* _font, unsigned int _clickEventId);
  Button(Canvas* _parent, unsigned int pos, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor, const char* _text, const FONT_INFO* _font, unsigned int _clickEventId);
};

#endif

