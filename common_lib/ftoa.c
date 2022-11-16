#include <ftoa.h>
#include <ultoa.h>

typedef union
{
  long	L;
  float	F;
} LF_t;

int ftoa(float f, char *buffer, int buffer_length)
{
  long mantissa, int_part, frac_part;
  short exp2;
  LF_t x;
  char m, max;

  if (f == 0.0)
  {
    if (buffer_length < 4)
      return _FTOA_BUFFER_TOO_SMALL;
    *buffer++ = '0';
    *buffer++ = '.';
    *buffer++ = '0';
    *buffer = 0;
    return 0;
  }
  
  x.F = f;

  exp2 = (unsigned char)(x.L >> 23) - 127;
  mantissa = (x.L & 0xFFFFFF) | 0x800000;
  frac_part = 0;
  int_part = 0;

  if (exp2 >= 31)
    return _FTOA_TOO_LARGE;
  else if (exp2 < -23)
    return _FTOA_TOO_SMALL;
  else if (exp2 >= 23)
    int_part = mantissa << (exp2 - 23);
  else if (exp2 >= 0) 
  {
    int_part = mantissa >> (23 - exp2);
    frac_part = (mantissa << (exp2 + 1)) & 0xFFFFFF;
  }
  else /* if (exp2 < 0) */
    frac_part = (mantissa & 0xFFFFFF) >> -(exp2 + 1);

  if (x.L < 0)
  {
    if (buffer_length-- > 0)
      *buffer++ = '-';
    else
      return _FTOA_BUFFER_TOO_SMALL;
  }

  if (int_part == 0)
    *buffer++ = '0';
  else
  {
    exp2 = ultoa(int_part, buffer, 10, buffer_length);
    if (exp2 < 0)
      return _FTOA_BUFFER_TOO_SMALL;
    buffer += exp2;
    buffer_length -= exp2;
  }

  if (buffer_length-- > 0)
    *buffer++ = '.';
  else
    return _FTOA_BUFFER_TOO_SMALL;

  if (frac_part == 0)
  {
    if (buffer_length-- > 0)
      *buffer++ = '0';
    else
      return _FTOA_BUFFER_TOO_SMALL;
  }
  else
  {
    if (buffer_length <= 0)
      return _FTOA_BUFFER_TOO_SMALL;
    max = buffer_length;
    if (max > 7)
      max = 7;
    /* print BCD */
    for (m = 0; m < max; m++)
    {
      /* frac_part *= 10;	*/
      frac_part = (frac_part << 3) + (frac_part << 1); 

      *buffer++ = (frac_part >> 24) + '0';
      frac_part &= 0xFFFFFF;
    }
    buffer_length -= max - 1;
    /* delete ending zeroes */
    for (--buffer; *buffer == '0' && buffer[-1] != '.'; --buffer) buffer_length++;
    ++buffer;
    buffer_length--;
  }
  
  if (buffer_length-- > 0)
    *buffer = 0;
  else
    return _FTOA_BUFFER_TOO_SMALL;

  return 0;
}
