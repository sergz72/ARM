#include <read_hex_string.h>
#include <string.h>

int read_hex_string(char *s, unsigned char *destination, int destination_size)
{
  int l = (int)strlen(s);
  int i;
  unsigned char c, v;

  if (!l || (l & 1))
    return -1;

  l >>= 1;
  if (l > destination_size)
    return -2;

  while (*s)
  {
    v = 0;
    for (i = 0; i < 2; i++)
    {
      v <<= 4;
      c = *s++;
      if (c < '0')
        return -1;
      if (c <= '9')
        v |= c - '0';
      else if (c < 'A')
        return -1;
      else if (c <= 'F')
        v |= c - 'A' + 10;
      else if (c < 'a' || c > 'f')
        return -1;
      else
        v |= c - 'a' + 10;
    }
    *destination++ = v;
  }

  return l;
}
