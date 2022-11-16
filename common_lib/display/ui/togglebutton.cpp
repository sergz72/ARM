#define PROJECT_EXPORTS
#include "togglebutton.h"

ToggleButton::ToggleButton(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor,
                           unsigned int _selectedColor, unsigned int _textColor, const char* _text, const FONT_INFO* _font, unsigned int _clickEventId) :
  Button(_parent, pos, _x, _y, _width, _height, _bkColor, _disabledBkColor, _textColor, _text, _font, _clickEventId)
{
  unselectedColor = _bkColor;
  selectedColor = _selectedColor;
  isSelected = 0;
}

ToggleButton::ToggleButton(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor,
                           unsigned int _selectedColor, unsigned int _textColor, const char* _text, const FONT_INFO* _font, unsigned int _clickEventId) :
  ToggleButton(_parent, pos, _x, _y, _width, _height, _bkColor, COLOR_TRANSPARENT, _selectedColor, _textColor, _text, _font, _clickEventId)
{

}

unsigned int ToggleButton::Toggle()
{
  isSelected = !isSelected;

  if (isSelected)
    SetBkColor(selectedColor);
  else
    SetBkColor(unselectedColor);

  return isSelected;
}
