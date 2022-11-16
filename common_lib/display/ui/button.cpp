#define PROJECT_EXPORTS
#include <button.h>

Button::Button(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor,
               const char* _text, const FONT_INFO* _font, unsigned int _clickEventId): Label(_parent, pos, _x, _y, _width, _height, _bkColor, _disabledBkColor, _textColor, _text, _font, ALIGN_CENTER)
{
  SetClickEventId(_clickEventId);
}

Button::Button(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _textColor,
               const char* _text, const FONT_INFO* _font, unsigned int _clickEventId) :
  Button(_parent, pos, _x, _y, _width, _height, _bkColor, COLOR_TRANSPARENT, _textColor, _text, _font, _clickEventId)
{

}

Button::Button(Canvas* _parent, unsigned int pos, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor, const char* _text, const FONT_INFO* _font, unsigned int _clickEventId):
  Button(_parent, pos, 0, 0, 0, 0, _bkColor, _disabledBkColor, _textColor, _text, _font, _clickEventId)
{

}