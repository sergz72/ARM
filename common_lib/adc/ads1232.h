#ifndef ADC_ADS1232_H
#define ADC_ADS1232_H

#define ADS1232_GAIN_1   0
#define ADS1232_GAIN_2   1
#define ADS1232_GAIN_64  2
#define ADS1232_GAIN_128 3

#define ADS1232_CHANNEL_AINP1_AINN1 0
#define ADS1232_CHANNEL_AINP2_AINN2 1
#define ADS1232_CHANNEL_TEMP_SENSOR 3

void ads1232_spi_transfer(int channel, unsigned char *rdata);// should be defined in hal.c

void ads1232_set_gain(int channel, int gain);
void ads1232_set_channel(int channel, int channel_no);
int ads1232_read(int channel, int *pvalue, int timeout);

#endif
