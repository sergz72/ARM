#ifndef I2C_SOFT_H
#define I2C_SOFT_H

void i2c_soft_init(int channel);
void i2c_soft_start(int channel);
void i2c_soft_stop(int channel);
unsigned char i2c_soft_rx(int channel, unsigned int ack);
unsigned int i2c_soft_tx(int channel, unsigned char d);
int i2c_soft_command(int channel, unsigned int address, unsigned char commands[],  unsigned int commands_length,
                     unsigned char out_data[], unsigned int out_data_length,
                     unsigned char in_data[], unsigned int in_data_length);
int i2c_soft_read(int channel, unsigned int address, unsigned char *in_data, unsigned int in_data_length);

#endif
