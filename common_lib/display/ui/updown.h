#ifndef _UPDOWN_H
#define _UPDOWN_H

#include "button.h"
#include "numericlabel.h"

#define UD_ORIENTATION_H 0
#define UD_ORIENTATION_V 1

class API UpDownControl : public Canvas
{
  NumericLabel* label;
  Button *upButton, *downButton;
public:
  UpDownControl(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _orientation,
                unsigned int _labelBkColor, unsigned int _labelDisabledBkColor, unsigned int _labelTextColor, const char* _unitName, const FONT_INFO* _labelFont, unsigned int _scale, unsigned int value,
                unsigned int buttonSize, unsigned int _buttonBkColor, unsigned int _buttonDisabledBkColor, unsigned int _buttonTextColor, const char* _upButtonText,
                const char* _downButtonText, const FONT_INFO* _buttonFont, unsigned int upClickEventId, unsigned int downClickEventId);
  UpDownControl(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _orientation,
                unsigned int _labelBkColor, unsigned int _labelTextColor, const char* _unitName, const FONT_INFO* _labelFont, unsigned int _scale, unsigned int value,
                unsigned int buttonSize, unsigned int _buttonBkColor, unsigned int _buttonTextColor, const char* _upButtonText,
                const char* _downButtonText, const FONT_INFO* _buttonFont, unsigned int upClickEventId, unsigned int downClickEventId);
  void SetValue(unsigned int value);
};

#endif
