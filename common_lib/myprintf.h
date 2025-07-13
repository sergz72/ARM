#ifndef _MYPRINTF_H
#define _MYPRINTF_H

#include <stdarg.h>

void put_string(const char *s, void (* writer)(char, void*), void *data);
int print_string(const char *str, int width, char padding, int pad_right, void (* writer)(char, void*), void *data);
int myvprintf(const char *format, void (*writer)(char, void*), va_list vArgs, void *data);
int mysprintf(char *s, const char *format, ...);
int myvsprintf(char *s, const char *format, va_list vArgs);


#endif
