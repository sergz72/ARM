#include <getstring.h>
#include <shell.h>
#include <stdio.h>

static char *b, *buf;
static int idx, blen;
static void (*putsf_)(const char *);
static int (*getcharf_)(void);

void getstring_init(char *buffer, int buffer_length, int (*getchar_)(void), void (*puts_)(const char *))
{
  idx = 0;
  b = buf = buffer;
  blen = buffer_length;
  putsf_ = puts_;
  getcharf_ = getchar_;
}

void getstring_buffer_init(const char *buffer_init)
{
  if (buffer_init)
  {
    putsf_(buffer_init);
    while (*buffer_init)
    {
      *buf++ = *buffer_init++;
      idx++;
    }
  }
}

int getstring_next(void)
{
  int c = 0, c2;
  char s[3];

  while (c != '\r')
  {
    c = getcharf_();
    //printf("0x%02X\n", c);
    switch (c)
    {
      case EOF:
        return EOF;
      case '\r':
        s[0] = c;
        s[1] = '\n';
        s[2] = 0;
        putsf_(s);
        break;
      case 0x1B:
        c = getcharf_();
        //printf("0x%02X\n", c);
        c2 = getcharf_();
        //printf("0x%02X\n", c2);
        if (c == 0x5B)
        {
          switch (c2)
          {
            case 0x41: // UP
              *b = SHELL_UP_KEY;
              buf = b;
              idx = 0;
              return 0;
            case 0x42: // DOWN
              *b = SHELL_DOWN_KEY;
              buf = b;
              idx = 0;
              return 0;
          }
        }
        break;
      case 8:
      case 0x7F:
        if (idx)
        {
          s[0] = c;
          s[1] = 0;
          putsf_(s);
          buf--;
          idx--;
        }
        break;
      default:
        if (idx < blen - 1)
        {
          s[0] = c;
          s[1] = 0;
          putsf_(s);
          *buf++ = c;
          idx++;
        }
        break;
    }
  }
  *buf = 0;
  buf = b;
  idx = 0;
  return 0;
}
