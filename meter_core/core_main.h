#ifndef CORE_MAIN_H
#define CORE_MAIN_H

#define I2C_SOFT
#define I2C_TIMEOUT 1000000
#define i2c_dly delay_us(5)

#define SI5351_XTAL_FREQ 25000000
#define SI5351_CHANNELS 4

#define MAX_SUBDEVICES 4

struct _DeviceObject;

int I2CCheck(int idx, int device_id);
void SCL_HIGH(int);
void SCL_LOW(int);
void SDA_HIGH(int);
void SDA_LOW(int);
int SDA_IN(int);
int SCL_IN(int);
void configure_hal(void);
void blink_led(void);
void main_comm_port_write_bytes(const unsigned char *data, int len);
int main_comm_port_read_bytes(unsigned char *buffer, int buffer_size);
unsigned long long int time_us(void);
void delay_us(unsigned int us);
void core_main(void);
void release_reset(void);
void change_channel(int);
int i2c_transfer(struct _DeviceObject *o, const void *txdata, unsigned int txdatalength, void *rxdata,
                        unsigned int rxdatalength);
int spi_transfer(struct _DeviceObject *o, const void *txdata, unsigned int txdatalength, void *rxdata,
                        unsigned int rxdatalength);
void init_spi(int module_id);
void init_interrupt_pin(const struct _DeviceObject *o);
int get_interrupt_pin_level(const struct _DeviceObject *o);

#endif
