#define PROJECT_EXPORTS
#include "graph.h"

Graph::Graph(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor,
             const FONT_INFO* _font):
  Canvas(_parent, pos, _x, _y, _width, _height, _bkColor, _disabledBkColor)
{
  textColor = _textColor;
  font = _font;
}

Graph::Graph(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _textColor, const FONT_INFO* _font) :
  Graph(_parent, pos, _x, _y, _width, _height, _bkColor, COLOR_TRANSPARENT, _textColor, _font)
{

}

unsigned int Graph::GetTextColor()
{
  return textColor;
}

const FONT_INFO* Graph::GetFont()
{
  return font;
}

void Graph::InternalDraw(GDevice* d)
{
  Canvas::InternalDraw(d);
}
