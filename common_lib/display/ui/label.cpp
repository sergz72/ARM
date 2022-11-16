#define PROJECT_EXPORTS
#include <label.h>

Label::Label(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor,
             const char* _text, const FONT_INFO* _font, unsigned int _textAlign): Canvas(_parent, pos, _x, _y, _width, _height, _bkColor, _disabledBkColor)
{
  text = _text;
  textColor = _textColor;
  font = _font;
  textAlign = _textAlign;
  if (GetHeight() == 0)
    SetHeight(font->char_height);
  CalculateWidth();
}

void Label::CalculateWidth()
{
  if (GetWidth() == 0 && text != NULL)
    SetWidth(FontGetWidth(text, font));
}

Label::Label(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _textColor,
  const char* _text, const FONT_INFO* _font, unsigned int _textAlign) :
  Label(_parent, pos, _x, _y, _width, _height, _bkColor, COLOR_TRANSPARENT, _textColor, _text, _font, _textAlign)
{

}

Label::Label(Canvas* _parent, unsigned int pos, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor, const char* _text, const FONT_INFO* _font, unsigned int _textAlign) :
  Label(_parent, pos, 0, 0, 0, 0, _bkColor, _disabledBkColor, _textColor, _text, _font, _textAlign)
{
}

Label::Label(Canvas* _parent, unsigned int pos, unsigned int _textColor, const char* _text, const FONT_INFO* _font, unsigned int _textAlign) :
  Label(_parent, pos, 0, 0, 0, 0, COLOR_TRANSPARENT, COLOR_TRANSPARENT, _textColor, _text, _font, _textAlign)
{
}

void Label::SetText(const char* _text)
{
  text = _text;
  SetUpdated();
}

void Label::InternalDraw(GDevice* d)
{
  int y;
  int h = GetHeight();

  Canvas::InternalDraw(d);

  if (h <= font->char_height)
    y = 0;
  else
    y = (h - font->char_height) / 2;

  switch (textAlign)
  {
  case ALIGN_LEFT:
    DrawTextLeft(d, 0, y, text, font, textColor);
    break;
  case ALIGN_CENTER:
    DrawTextCenter(d, 0, y, text, font, textColor, GetWidth());
    break;
  case ALIGN_RIGHT:
    DrawTextRight(d, 0, y, text, font, textColor, GetWidth());
    break;
  }
}
