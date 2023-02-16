#ifndef _MAX7219_H_
#define _MAX7219_H_

void max7219_send(unsigned short **data);

void max7219_init(unsigned short intensity);
void max7219_out(const unsigned char *data);
void max7219_intensity(unsigned short intensity);
void max7219_on(void);
void max7219_off(void);
void max7219_display_test_on(void);
void max7219_display_test_off(void);
void max7219_scan_limit(unsigned short limit);
void max7219_decode_mode(unsigned short mode);
void max7219_clear_screen(void);

#endif
