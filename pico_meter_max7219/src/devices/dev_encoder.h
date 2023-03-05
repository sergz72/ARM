#ifndef PICO_METER_MAX7219_DEV_ENCODER_H
#define PICO_METER_MAX7219_DEV_ENCODER_H

void dev_encoder_init(void);
unsigned int encoder_get_status(void);
int process_encoder_event(unsigned int encoder_status);

#endif //PICO_METER_MAX7219_DEV_ENCODER_H
