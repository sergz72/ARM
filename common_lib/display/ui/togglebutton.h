#ifndef _TOGGLE_BUTTON_H
#define _TOGGLE_BUTTON_H

#include "button.h"

class API ToggleButton : public Button
{
  unsigned int unselectedColor, selectedColor, isSelected;
public:
  ToggleButton(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor,
               unsigned int _selectedColor, unsigned int _textColor, const char* _text, const FONT_INFO* _font, unsigned int _clickEventId);
  ToggleButton(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor,
               unsigned int _selectedColor, unsigned int _textColor, const char* _text, const FONT_INFO* _font, unsigned int _clickEventId);
  unsigned int Toggle();
};

#endif
