#ifndef _NUMERICLABEL_H
#define _NUMERICLABEL_H

#include "label.h"

class API NumericLabel : public Label
{
  const char* unitName;
  unsigned int divider;
  char valueText[30], format[15];
public:
  NumericLabel(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor,
               const char *_unitName, const FONT_INFO* _font, unsigned int _textAlign, unsigned int _scale, unsigned int value);
  NumericLabel(Canvas* _parent, unsigned int pos, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor, const char* _unitName, const FONT_INFO* _font, unsigned int _textAlign,
               unsigned int _scale, unsigned int value);
  NumericLabel(Canvas* _parent, unsigned int pos, unsigned int _textColor, const char* _unitName, const FONT_INFO* _font, unsigned int _textAlign, unsigned int _scale, unsigned int value);
  void SetValue(unsigned int value);
};

#endif
