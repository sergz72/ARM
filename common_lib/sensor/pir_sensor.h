#ifndef _PIR_SENSOR_H
#define _PIR_SENSOR_H

void pir_sensor_adc_handler(unsigned short value);
void pir_sensor_init(void);

void pir_motion_detected(void); // should be defined somewhere

extern volatile unsigned short pir_filter_output;
extern volatile unsigned short pir_adc_samples_avg;

#endif
