#ifndef _GETSTRING_H
#define _GETSTRING_H

void getstring_init(char *buffer, int buffer_length, int (*getchar_)(void), void (*puts_)(const char *));
void getstring_buffer_init(const char *buffer_init);
int getstring_next(void);

#endif
