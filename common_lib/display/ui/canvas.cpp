#define PROJECT_EXPORTS
#include <display.h>
#include <canvas.h>

Canvas::Canvas(Canvas* _parent, unsigned int pos, unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height, unsigned int _bkColor, unsigned int _disabledBkColor)
{
  next = firstChild = lastChild = NULL;
  parent = _parent;
  width = _width;
  height = _height;
  bkColor = _bkColor;
  disabledBkColor = _disabledBkColor;
  if (_parent)
  { 
    left = _parent->left + _x;
    top = _parent->top + _y;
    parent->AddChild(this, pos);
  }
  else
  {
    left = _x;
    top = _y;
  }

  clickEventId = EV_CLICK;
  isDisabled = 0;

  updated = 1;
}

unsigned int Canvas::GetBkColor()
{
  int c = isDisabled ? disabledBkColor : bkColor;
  Canvas *cv = this;
  while (c == COLOR_TRANSPARENT && cv->parent)
  {
    cv = cv->parent;
    c = isDisabled ? cv->disabledBkColor : cv->bkColor;
  }

  return c;
}

int Canvas::isInside(unsigned int x, unsigned int y)
{
  return y < height && x < width;
}

int Canvas::isPointInside(ts_point *p)
{
  return p->x >= left && p->x < left + width && p->y >= top && p->y < top + height;
}

void Canvas::AddChild(Canvas* child, unsigned int pos)
{
  Canvas* c = lastChild;
  lastChild = child;
  if (c)
  {
    c->next = child;
    if (pos & POS_RESTV)
    {
      pos &= ~POS_RESTV;
      child->height = height - (c->top + c->height - top) - 1;
    }
    switch (pos)
    {
    case POS_NEXTH:
      child->left = c->left + c->width + child->left - left;
      child->top = c->top;
      break;
    case POS_NEXTV:
      child->top = c->top + c->height + child->top - top;
      break;
    case POS_RESTH:
      child->left = c->left + c->width + child->left;
      child->top = c->top;
      child->width = width - (child->left - left);
      break;
    case POS_RESTHM1:
      child->left = c->left + c->width;
      child->top = c->top;
      child->width = width - (child->left - left) - 1;
      break;
    }
  }
  if (!firstChild)
    firstChild = child;
}

void Canvas::WriteTextData(GDevice* d, unsigned int x, unsigned int y, const unsigned char* bitmap, unsigned int height,
                           unsigned int bit, unsigned int char_bytes, unsigned int textColor, unsigned int bkColor)
{
  if (bitmap != NULL)
    bitmap += bit >> 3U;
  bit = 1U << (7U - (bit % 8U));

  x += left;
  y += top;
  while (height)
  {
    if (bitmap != NULL && (*bitmap & bit))
      d->DrawPixel(x, y, textColor);
    else
      d->DrawPixel(x, y, bkColor);

    height--;
    y++;
    bitmap += char_bytes;
  }
}

unsigned int Canvas::DrawTextLeft(GDevice *d, unsigned int x, unsigned int y, const char* text, const FONT_INFO* f, unsigned int textColor)
{
  char c;
  unsigned int col, cwidth, max_width, w, h, bit, character_bytes, bkColor;
  const unsigned char* bitmap;
  FONT_CHAR_INFO *pfci;

  if (!isInside(x, y) || !*text)
   return 0;

  bkColor = GetBkColor();

  col = x;
  pfci = d->GetFCI();
  cwidth = 0;
  max_width = width - col;

  while ((c = *text++) != 0 && max_width)
  {
    if (FontGetChar(c, f, pfci))
    {
      w = pfci->char_width > max_width ? max_width : pfci->char_width;
      col += w;
      cwidth += w;
      max_width -= w;
    }
    pfci++;
    if (max_width)
    {
      w = f->character_spacing > max_width ? max_width : f->character_spacing;
      col += w;
      cwidth += w;
      max_width -= w;
    }
  }

  pfci = d->GetFCI();
  bitmap = f->char_bitmaps + pfci->char_offset;
  h = y + f->char_height > height ? height - y : f->char_height;
  w = cwidth;
  bit = 0;
  character_bytes = pfci->char_width >> 3U;
  if (pfci->char_width & 7U)
    character_bytes++;

  while (w--)
  {
    WriteTextData(d, x++, y, bitmap, h, bit++, character_bytes, textColor, bkColor);
    if (bit >= pfci->char_width)
    {
      if (!w)
        break;
      for (bit = 0; bit < f->character_spacing; bit++)
      {
        WriteTextData(d, x++, y, 0, h, 0, 0, textColor, bkColor);
        w--;
        if (!w)
          return cwidth;
      }
      pfci++;
      character_bytes = pfci->char_width >> 3;
      if (pfci->char_width & 7)
        character_bytes++;
      bitmap = f->char_bitmaps + pfci->char_offset;
      bit = 0;
    }
  }

  return cwidth;
}

unsigned int Canvas::DrawTextCenter(GDevice* d, unsigned int x, unsigned int y, const char* text, const FONT_INFO* f, unsigned int textColor, unsigned int width)
{
  if (!*text)
    return 0;
  x += (width - FontGetWidth(text, f)) / 2;
  return DrawTextLeft(d, x, y, text, f, textColor);
}

unsigned int Canvas::DrawTextRight(GDevice* d, unsigned int x, unsigned int y, const char* text, const FONT_INFO* f, unsigned int textColor, unsigned int width)
{
  if (!*text)
    return 0;
  x += width - FontGetWidth(text, f);
  return DrawTextLeft(d, x, y, text, f, textColor);
}

void Canvas::EraseBackground(GDevice* d)
{
  d->FillRect(left, top, width, height, GetBkColor());
}

void Canvas::InternalDraw(GDevice* d)
{
  EraseBackground(d);
}

unsigned int Canvas::DrawChildren(GDevice* d)
{
  Canvas* c = firstChild;
  unsigned int result = 0;

  while (c)
  {
    if (c->Draw(d))
      result = 1;
    c = c->next;
  }

  return result;
}

unsigned int Canvas::Draw(GDevice* d)
{
  unsigned int result = updated;
  if (result)
  {
    InternalDraw(d);
    updated = 0;
  }
  unsigned int result2 = DrawChildren(d);
  return result || result2;
}

int Canvas::isUpdated()
{
  return updated;
}

void Canvas::SetUpdated()
{
  updated = 1;
}

void Canvas::ResetUpdated()
{
  updated = 0;
}

unsigned int Canvas::GetWidth()
{
  return width;
}

unsigned int Canvas::GetHeight()
{
  return height;
}

void Canvas::DrawRect(GDevice* d, unsigned int x, unsigned int y, unsigned int _width, unsigned int _height, unsigned int color)
{
  d->DrawRect(left + x, top + y, _width, _height, color);
}

void Canvas::SetWidth(unsigned int w)
{
  width = w;
}

void Canvas::SetHeight(unsigned int h)
{
  height = h;
}

unsigned int Canvas::GetTop()
{
  return top;
}

unsigned int Canvas::GetLeft()
{
  return left;
}

unsigned int Canvas::Click(ts_point* p)
{
  Canvas* child;
  unsigned int rc;

  if (isPointInside(p))
  {
    child = firstChild;
    while (child)
    {
      rc = child->Click(p);
      if (rc)
        return rc;
      child = child->next;
    }
    return isDisabled ? EV_CLICK : clickEventId;
  }
  return 0;
}

void Canvas::SetBkColor(unsigned int color)
{
  bkColor = color;
  updated = 1;
}

void Canvas::SetClickEventId(unsigned int id)
{
  clickEventId = id;
}

void Canvas::Disable()
{
  if (!isDisabled)
  {
    isDisabled = 1;
    updated = 1;
    Canvas* child = firstChild;
    while (child)
    {
      child->Disable();
      child = child->next;
    }
  }
}

void Canvas::Enable()
{
  if (isDisabled)
  {
    isDisabled = 0;
    updated = 1;
    Canvas* child = firstChild;
    while (child)
    {
      child->Enable();
      child = child->next;
    }
  }
}
