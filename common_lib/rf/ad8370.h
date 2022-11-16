#ifndef AD8370_H_
#define AD8370_H_

#define AD8370_MIN_GAIN 0.055744
#define AD8370_MAX_GAIN 50

void AD8370_SetGain(int spi_id, float gain);

#endif /*AD8370_H_*/
