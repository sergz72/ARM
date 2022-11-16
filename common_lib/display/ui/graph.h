#ifndef _GRAPH_H
#define _GRAPH_H

#include "canvas.h"

class API Graph : public Canvas
{
  unsigned int textColor;
  const FONT_INFO* font;
protected:
  unsigned int GetTextColor();
  const FONT_INFO* GetFont();
  virtual void InternalDraw(GDevice* d);
public:
  Graph(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor, unsigned int _textColor,
        const FONT_INFO* _font);
  Graph(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _textColor, const FONT_INFO* _font);
};

#endif
