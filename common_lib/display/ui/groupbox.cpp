#define PROJECT_EXPORTS
#include "groupbox.h"

GroupBox::GroupBox(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor,
                   unsigned int _textColor, const char* _text, const FONT_INFO* _font, unsigned int _frameColor): Canvas(_parent, pos, _x, _y, _width, _height, _bkColor, _disabledBkColor)
{
  text = _text;
  textColor = _textColor;
  font = _font;
  frameColor = _frameColor;
}

GroupBox::GroupBox(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _textColor,
                   const char* _text, const FONT_INFO* _font, unsigned int _frameColor):
  GroupBox(_parent, pos, _x, _y, _width, _height, _bkColor, COLOR_TRANSPARENT, _textColor, _text, _font, _frameColor)
{

}

void GroupBox::InternalDraw(GDevice* d)
{
  DrawRect(d, 0, 0, GetWidth(), GetHeight(), frameColor);
  DrawTextLeft(d, 0, 0, text, font, textColor);
}

void GroupBox::SetText(const char* _text)
{
  text = _text;
  SetUpdated();
}