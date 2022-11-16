#define PROJECT_EXPORTS
#include "updown.h"

UpDownControl::UpDownControl(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _orientation,
  unsigned int _labelBkColor, unsigned int _labelDisabledBkColor, unsigned int _labelTextColor, const char* _unitName, const FONT_INFO* _labelFont, unsigned int _scale, unsigned int value,
  unsigned int buttonSize, unsigned int _buttonBkColor, unsigned int _buttonDisabledBkColor, unsigned int _buttonTextColor, const char* _upButtonText,
  const char* _downButtonText, const FONT_INFO* _buttonFont, unsigned int upClickEventId, unsigned int downClickEventId) :
  Canvas(_parent, pos, _x, _y, _width, _height, COLOR_TRANSPARENT, COLOR_TRANSPARENT)
{
  if (_orientation == UD_ORIENTATION_H)
  {
    label = new NumericLabel(this, POS_ABSOLUTE, _x + buttonSize, _y, _width - buttonSize - buttonSize, _height, _labelBkColor, _labelDisabledBkColor, _labelTextColor, _unitName, _labelFont, ALIGN_CENTER,
                             _scale, value);
    downButton = new Button(this, POS_ABSOLUTE, _x, _y, buttonSize, _height, _buttonBkColor, _buttonDisabledBkColor, _buttonTextColor, _downButtonText, _buttonFont, downClickEventId);
    upButton = new Button(this, POS_ABSOLUTE, _x + _width - buttonSize, _y, buttonSize, _height, _buttonBkColor, _buttonDisabledBkColor, _buttonTextColor, _upButtonText, _buttonFont, upClickEventId);
  }
  else
  {
    label = new NumericLabel(this, POS_ABSOLUTE, _x, _y + buttonSize, _width, _height - buttonSize - buttonSize, _labelBkColor, _labelDisabledBkColor, _labelTextColor, _unitName, _labelFont, ALIGN_CENTER,
                             _scale, value);
    downButton = new Button(this, POS_ABSOLUTE, _x, _y, _width, buttonSize, _buttonBkColor, _buttonDisabledBkColor, _buttonTextColor, _downButtonText, _buttonFont, downClickEventId);
    upButton = new Button(this, POS_ABSOLUTE, _x, _y + _height - buttonSize, _width, buttonSize, _buttonBkColor, _buttonDisabledBkColor, _buttonTextColor, _upButtonText, _buttonFont, upClickEventId);
  }
}

UpDownControl::UpDownControl(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _orientation,
                             unsigned int _labelBkColor, unsigned int _labelTextColor, const char* _unitName, const FONT_INFO* _labelFont, unsigned int _scale, unsigned int value,
                             unsigned int buttonSize, unsigned int _buttonBkColor, unsigned int _buttonTextColor, const char* _upButtonText,
                             const char* _downButtonText, const FONT_INFO* _buttonFont, unsigned int upClickEventId, unsigned int downClickEventId) :
  UpDownControl(_parent, pos, _x, _y, _width, _height, _orientation, _labelBkColor, COLOR_TRANSPARENT, _labelTextColor, _unitName, _labelFont, _scale, value,
                buttonSize, _buttonBkColor, COLOR_TRANSPARENT, _buttonTextColor, _upButtonText, _downButtonText, _buttonFont, upClickEventId, downClickEventId)
{

}

void UpDownControl::SetValue(unsigned int value)
{
  label->SetValue(value);
}
