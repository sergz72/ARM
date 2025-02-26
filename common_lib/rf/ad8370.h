#ifndef AD8370_H_
#define AD8370_H_

#define AD8370_MIN_GAIN -25
#define AD8370_MAX_GAIN 34

int AD8370_SetGain(int spi_id, int gain); // gain is in dB
void ad8370_spi_send(int spi_id, unsigned char code);

#endif
