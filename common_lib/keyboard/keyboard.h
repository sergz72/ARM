#ifndef KEYBOARD_H_
#define KEYBOARD_H_

void KbdGpioInit(void);
void KbdSet(int state);
int KbdGet(void);
void KbdLocksUpdated(int Locks);
int KbdIsInterruptValid(void);

typedef unsigned char (*KbdHandler)(void);

void KbdInit(const unsigned char *KMap, const unsigned char *LKeys, int LKeyCount, KbdHandler handler);
char KbdCheck(void);
unsigned char MatrixKbdHandler(int set_after);

#endif /*KEYBOARD_H_*/
