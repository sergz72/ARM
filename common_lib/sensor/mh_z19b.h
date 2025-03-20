#ifndef MH_Z19B_H
#define MH_Z19B_H

void mh_z19b_set_range(int hi_range);
void mh_z19b_send_read_command(void);
int mh_z19b_get_result(unsigned short *result);

void mh_z19b_send(unsigned char *data, int len); // should be defined in hal.c
int mh_z19b_read(unsigned char *data, int data_size); // should be defined in hal.c

#endif
