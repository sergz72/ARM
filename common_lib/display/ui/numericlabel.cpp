#define PROJECT_EXPORTS
#include <numericlabel.h>
#include <stdio.h>

NumericLabel::NumericLabel(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor,
                           unsigned int _textColor, const char* _unitName, const FONT_INFO* _font, unsigned int _textAlign, unsigned int _scale, unsigned int value) :
  Label(_parent, pos, _x, _y, _width, _height, _bkColor, _disabledBkColor, _textColor, NULL, _font, _textAlign)
{
  unitName = _unitName;
  sprintf_s(format, sizeof(format), "%%d.%%0%dd%%s", _scale);
  divider = 1;
  while (_scale--)
    divider *= 10;
  SetText(valueText);
  SetValue(value);
  CalculateWidth();
}

NumericLabel::NumericLabel(Canvas* _parent, unsigned int pos, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor, const char* _unitName, const FONT_INFO* _font,
                           unsigned int _textAlign, unsigned int _scale, unsigned int value) :
  NumericLabel(_parent, pos, 0, 0, 0, 0, _bkColor, _disabledBkColor, _textColor, _unitName, _font, _textAlign, _scale, value)
{

}

NumericLabel::NumericLabel(Canvas* _parent, unsigned int pos, unsigned int _textColor, const char* _unitName, const FONT_INFO* _font, unsigned int _textAlign, unsigned int _scale, unsigned int value)
  : NumericLabel(_parent, pos, 0, 0, 0, 0, COLOR_TRANSPARENT, COLOR_TRANSPARENT, _textColor, _unitName, _font, _textAlign, _scale, value)
{

}

void NumericLabel::SetValue(unsigned int value)
{
  if (divider > 1)
    sprintf_s(valueText, sizeof(valueText), format, value / divider, value % divider, unitName);
  else
    sprintf_s(valueText, sizeof(valueText), "%d%s", value, unitName);
  SetUpdated();
}
