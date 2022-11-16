#include <ultoa.h>

short ultoa(unsigned long v, char *string, int radix, int buffer_length)
{
  char tmp[33];
  char *tp = tmp;
  int i;
  short result = 0;

  while (v || tp == tmp)
  {
    i = v % radix;
    v = v / radix;
    if (i < 10)
      *tp++ = i+'0';
    else
      *tp++ = i + 'A' - 10;
    result++;
  }

  if (result + 1 > buffer_length)
    return -1;

  while (tp > tmp)
    *string++ = *--tp;
  *string = 0;

  return result;
}
