#ifndef _CC1101_FUNC_H
#define _CC1101_FUNC_H

int cc1101Init(unsigned char device_address, unsigned char packet_length, unsigned int frequency);
int cc1101Receive(unsigned char **buffer, unsigned char *size);
int cc1101Send(unsigned char host_address, unsigned char *data, unsigned char length);
int cc1101ReceiveStart(void);
int cc1101ReceiveStop(void);

extern volatile int packet_received;

#endif
