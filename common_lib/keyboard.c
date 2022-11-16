#include <keyboard.h>
#include "board.h"

unsigned int KbdCmd(unsigned int kbd_pin, unsigned int cmd)
{
  switch (kbd_pin)
  {
#ifdef KBD_1CLR
    case 1:
      switch (cmd)
      {
        case KBD_CLR: KBD_1CLR; break;
        case KBD_OFF: KBD_1OFF; break;
        case KBD_GET: return KBD_1GET;
      }
      break;
#endif
#ifdef KBD_2CLR
    case 2:
      switch (cmd)
      {
        case KBD_CLR: KBD_2CLR; break;
        case KBD_OFF: KBD_2OFF; break;
        case KBD_GET: return KBD_2GET;
      }
      break;
#endif
#ifdef KBD_3CLR
    case 3:
      switch (cmd)
      {
        case KBD_CLR: KBD_3CLR; break;
        case KBD_OFF: KBD_3OFF; break;
        case KBD_GET: return KBD_3GET;
      }
      break;
#endif
#ifdef KBD_4CLR
    case 4:
      switch (cmd)
      {
        case KBD_CLR: KBD_4CLR; break;
        case KBD_OFF: KBD_4OFF; break;
        case KBD_GET: return KBD_4GET;
      }
      break;
#endif
#ifdef KBD_5CLR
    case 5:
      switch (cmd)
      {
        case KBD_CLR: KBD_5CLR; break;
        case KBD_OFF: KBD_5OFF; break;
        case KBD_GET: return KBD_5GET;
      }
      break;
#endif
#ifdef KBD_6CLR
    case 6:
      switch (cmd)
      {
        case KBD_CLR: KBD_6CLR; break;
        case KBD_OFF: KBD_6OFF; break;
        case KBD_GET: return KBD_6GET;
      }
      break;
#endif
#ifdef KBD_7CLR
    case 7:
      switch (cmd)
      {
        case KBD_CLR: KBD_7CLR; break;
        case KBD_OFF: KBD_7OFF; break;
        case KBD_GET: return KBD_7GET;
      }
      break;
#endif
#ifdef KBD_8CLR
    case 8:
      switch (cmd)
      {
        case KBD_CLR: KBD_8CLR; break;
        case KBD_OFF: KBD_8OFF; break;
        case KBD_GET: return KBD_8GET;
      }
      break;
#endif
#ifdef KBD_9CLR
    case 9:
      switch (cmd)
      {
        case KBD_CLR: KBD_9CLR; break;
        case KBD_OFF: KBD_9OFF; break;
        case KBD_GET: return KBD_9GET;
      }
      break;
#endif
#ifdef KBD_10CLR
    case 10:
      switch (cmd)
      {
        case KBD_CLR: KBD_10CLR; break;
        case KBD_OFF: KBD_10OFF; break;
        case KBD_GET: return KBD_10GET;
      }
      break;
#endif
#ifdef KBD_11CLR
    case 11:
      switch (cmd)
      {
        case KBD_CLR: KBD_11CLR; break;
        case KBD_OFF: KBD_11OFF; break;
        case KBD_GET: return KBD_11GET;
      }
      break;
#endif
  }

  return 0;
}

unsigned short KbdCheck(void)
{
  unsigned short i, j;

  for (i = 1; i <= KBD_COUNT; i++)
  {
    KbdCmd(i, KBD_CLR);
//      delay(1);
    for (j = 1; j <= KBD_COUNT; j++)
    {
      if (j == i)
        continue;
      if (!KbdCmd(j, KBD_GET))
      {
        KbdCmd(i, KBD_OFF);
        return i | (j << 8);
      }
    }
    KbdCmd(i, KBD_OFF);
  }

  return 0;
}

unsigned short KbdState = 0;
unsigned short Locks = 0;
const unsigned short *KbdMap;
const unsigned short *KbdLockKeys;
unsigned short KbdLockKeyCount;

void KbdInit(const unsigned short *KMap, const unsigned short *LKeys, unsigned short LKeyCount)
{
  KbdMap = KMap;
  KbdLockKeys = LKeys;
  KbdLockKeyCount = LKeyCount;
}

char KbdGet(int check_eq)
{
  unsigned short state, i;
  const unsigned short *map;

  state = KbdCheck();
  if (state == KbdState && check_eq)
  {
    delay1(50000);
    return 0;
  }
  KbdState = state;
  if (!state)
  {
    if (check_eq)
      delay1(50000);
    return 0;
  }
/*  for (;;)
  {
    delay(50000);
    state = KbdCheck();
    if (state == 0)
    {
      KbdState = 0;
      return 0;
    }
    if (state == KbdState)
      break;
    KbdState = state;
  }*/ 

  map = KbdLockKeys;

  for (i = 0; i < KbdLockKeyCount; i++)
  {
    if (state == *map)
    {
      Locks ^= 1 << i;
      return 0;
    }
    map++;
  }

  map = KbdMap;
  i = (1 << KbdLockKeyCount) + 1;

  while (map[0])
  {
    if (state == map[0])
      return map[Locks + 1];
    map += i;
  }

  return 0;
}
