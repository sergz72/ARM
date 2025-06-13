#include "board.h"
#include <common_printf.h>
#include <stdio.h>
#include <stdarg.h>

int common_printf(const char *format, ...)
{
  static char buffer[PRINTF_BUFFER_LENGTH], buffer2[PRINTF_BUFFER_LENGTH];
  char *p, *p2;
  va_list vArgs;
  int rc;

  va_start(vArgs, format);
  rc = vsnprintf(buffer, sizeof(buffer), format, vArgs);
  va_end(vArgs);
  p = buffer;
  p2 = buffer2;
  int l = 0;
  while (*p)
  {
    if (*p == '\n')
    {
      *p2++ = '\r';
      l++;
    }
    *p2++ = *p++;
    l++;
  }
  *p2 = 0;
  puts_(buffer2);
  return rc;
}
