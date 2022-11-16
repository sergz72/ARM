#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#define KBD_CLR 0
#define KBD_OFF 1
#define KBD_GET 2

unsigned int KbdCmd(unsigned int kbd_pin, unsigned int cmd);
unsigned short KbdCheck(void);
void KbdInit(const unsigned short *KMap, const unsigned short *LKeys, unsigned short LKeyCount);
char KbdGet(int check_eq);

#endif /*KEYBOARD_H_*/
