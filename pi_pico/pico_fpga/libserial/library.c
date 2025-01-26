#include "library.h"
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <stdio.h>
#include <string.h>
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
//#include <stdio.h>

int serial_open(const char *file_name, int baud)
{
  // Create new termios struct, we call it 'tty' for convention
  struct termios tty;

  if (baud == 115200)
    baud = B115200;
  else
    return -1;

  //printf("serial_open %s %d\n", file_name, baud);
  int fd = open(file_name, O_RDWR|O_NOCTTY|O_NONBLOCK);

  // Read in existing settings, and handle any error
  int rc = tcgetattr(fd, &tty);
  if (rc != 0)
    return rc;

  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
  tty.c_cflag |= CS8; // 8 bits per byte (most common)
  tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO; // Disable echo
  tty.c_lflag &= ~ECHOE; // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

  tty.c_cc[VTIME] = 0;    // Wait for up to 0.1s (1 deciseconds), returning as soon as any data is received.
  tty.c_cc[VMIN] = 0;

  rc = cfsetispeed(&tty, baud);
  if (rc != 0)
    return rc;
  rc = cfsetospeed(&tty, baud);
  if (rc != 0)
    return rc;

  // Save tty settings, also checking for error
  rc = tcsetattr(fd, TCSANOW, &tty);
  if (rc != 0)
    return rc;
  return fd;
}

void serial_close(int fd)
{
  //printf("serial_close %d\n", fd);
  close(fd);
}

int serial_write(int fd, const void *buf, int count)
{
  //printf("serial_write %d %p %d\n", fd, buf, count);
  int n = write(fd, buf, count);
  fsync(fd);
  return n;
}

int serial_read(int fd, void *buf, int count)
{
  //printf("serial_read %d %p %d\n", fd, buf, count);
  return read(fd, buf, count);
}
