#ifndef LIBSERIAL_LIBRARY_H
#define LIBSERIAL_LIBRARY_H
#include <stddef.h>

int serial_open(const char *file_name, int baud);
void serial_close(int fd);
int serial_write(int fd, const void *buf, int count);
int serial_read(int fd, void *buf, int count);

#endif //LIBSERIAL_LIBRARY_H