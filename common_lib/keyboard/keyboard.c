#include <keyboard.h>
#include "board.h"

#define KBD_ALL1 ((1<<KBD_COUNTX)-1)

static int Locks = 0;
static const unsigned char *KbdMap;
static const unsigned char *KbdLockKeys;
static int KbdLockKeyCount;
static KbdHandler handler;

#ifndef CUSTOM_KBD_HANDLER
int KbdIsInterruptValid(void)
{
  return KbdGet() != KBD_ALL1;
}

unsigned char MatrixKbdHandler(int set_after)
{
  int i, j, rc, set;

  if (!set_after)
    KbdSet(0);
  rc = KbdGet();
  if (rc == KBD_ALL1)
    return 0;

  set = 1;
  for (i = 0; i < KBD_COUNTX; i++)
  {
    KbdSet(set);
    if (KbdGet() == KBD_ALL1)
    {
      j = 0;
      while (rc & 1)
      {
        rc >>= 1;
        j++;
      }
      if (set_after)
        KbdSet(0);
      return i * KBD_COUNTX + j + 1;
    }
    set <<= 1;
    set |= 1;
  }
  if (set_after)
    KbdSet(0);
  return 0;
}
#endif

char KbdCheck(void)
{
  int i;
  const unsigned char *map;
  unsigned char state = handler();

  if (!state)
    return 0;

  state--;

  map = KbdLockKeys;

  for (i = 0; i < KbdLockKeyCount; i++)
  {
    if (state == *map)
    {
      Locks ^= 1 << i;
      KbdLocksUpdated(Locks);
      return 0;
    }
    map++;
  }

  return KbdMap[state * (1 << KbdLockKeyCount) + Locks];
}

void KbdInit(const unsigned char *KMap, const unsigned char *LKeys, int LKeyCount, KbdHandler h)
{
  KbdMap = KMap;
  KbdLockKeys = LKeys;
  KbdLockKeyCount = LKeyCount;
  handler = h;
  KbdGpioInit();
}
