#ifndef FTOA_H_
#define FTOA_H_

#define _FTOA_TOO_LARGE 1
#define _FTOA_TOO_SMALL 2
#define _FTOA_BUFFER_TOO_SMALL 3

int ftoa(float f, char *buffer, int buffer_length);

#endif /*FTOA_H_*/
