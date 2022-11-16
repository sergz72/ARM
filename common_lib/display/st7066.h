#ifndef _ST7066_H
#define _ST7066_H

#define ST7066_ENTRY_MODE_ADDRESS_INCREMENT 2 // DDRAM address increment
#define ST7066_ENTRY_MODE_SHIFT_DISPLAY     1 // Shift of entire display
#define ST7066_DISPLAY_ON                   4
#define ST7066_CURSOR_ON                    2
#define ST7066_BLINK_ON                     1
#define ST7066_SHIFT_DISPLAY                8
#define ST7066_SHIFT_RIGHT                  4
#define ST7066_FUNCTION_N                   8
#define ST7066_FUNCTION_F                   4

#define ST066_LINE0_ADDRESS 0U
#define ST066_LINE1_ADDRESS 0x40U
#define ST066_LINE2_ADDRESS 0x14U
#define ST066_LINE3_ADDRESS 0x54U

void st7066_init(unsigned int num_rows, unsigned int mode);
void st7066_clear_display(unsigned int wait);
void st7066_return_home(void);
void st7066_entry_mode(unsigned int mode);
void st7066_display_control(unsigned int dcb);
void st7066_shift(unsigned int scrl);
void st7066_function_set(unsigned int nf);
void st7066_set_ddram_address(unsigned int address);
void st7066_write_data(unsigned int data);

#endif
