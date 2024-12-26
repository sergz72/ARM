#include "myprintf.h"
#include <stdlib.h>
#include <string.h>
#include <ultoa.h>

void put_string(const char *s, void (* writer)(char, void*), void *data)
{
  char c;

  while (1)
  {
    c = *s++;
    if (!c)
      break;
    writer(c, data);
  }
}

int print_string(const char *str, int width, char padding, int pad_right, void (* writer)(char, void*), void *data)
{
  int len = strlen(str);
  int rc;
  if (width <= len)
  {
    rc = len;
    width = 0;
  }
  else
  {
    rc = width;
    width -= len;
  }
  if (!pad_right)
  {
    while (width--)
      writer(padding, data);
  }

  put_string(str, writer, data);

  if (pad_right)
  {
    while (width--)
      writer(padding, data);
  }

  return rc;
}

int myvprintf(const char *format, void (*writer)(char, void*), const va_list vArgs, void *data)
{
  int count, v, width, pad_right;
  unsigned int u;
  char c, *s, padding, temp[20];

  if (format == NULL)
    return 0;

  count = 0;
  while (1)
  {
    c = *format++;
    if (!c)
      break;
    if (c != '%')
    {
      writer(c, data);
      count++;
    }
    else
    {
      padding = ' ';
      width = 0;
      c = *format++;
      pad_right = c == '-';
      if (pad_right)
        c = *format++;
      while (c >= '0' && c <= '9')
      {
        if (c == '0')
        {
          if (width == 0)
            padding = c;
          else
            width *= 10;
        }
        else
          width = width * 10 + c - '0';
        c = *format++;
      }
      switch (c)
      {
        case 0:
          return count;
        case '%':
          writer(c, data);
          count++;
          break;
        case 's':
          s = va_arg(vArgs, char*);
          if (s != NULL)
          {
            while (1)
            {
              c = *s++;
              if (!c)
                break;
              writer(c, data);
              count++;
            }
          }
          break;
        case 'd':
          v = va_arg(vArgs, int);
          itoa(v, temp, 10);
          count += print_string(temp, width, padding, pad_right, writer, data);
          break;
        case 'u':
          u = va_arg(vArgs, unsigned int);
          ultoa(u, temp, 10, sizeof(temp));
          count += print_string(temp, width, padding, pad_right, writer, data);
          break;
        case 'x':
          u = va_arg(vArgs, unsigned int);
          ultoa(u, temp, 16, sizeof(temp));
          count += print_string(temp, width, padding, pad_right, writer, data);
          break;
        default:
          break;
      }
    }
  }
  return count;
}

void savechar(char c, void *data)
{
  char *s = *(char**)data;
  *s++ = c;
  *(char**)data = s;
}

int mysprintf(char *s, const char *format, ...)
{
  int rc;
  va_list vArgs;

  if (s == NULL)
    return 0;

  va_start(vArgs, format);

  rc = myvprintf(format, savechar, vArgs, &s);

  va_end(vArgs);

  *s = 0;

  return rc;
}

int myvsprintf(char *s, const char *format, const va_list vArgs)
{
  int rc = myvprintf(format, savechar, vArgs, &s);

  *s = 0;

  return rc;
}
