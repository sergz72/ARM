#ifndef _EPD_SSD1680_H
#define _EPD_SSD1680_H

#define DATA_ENTRY_INCRY_INCRX 0b11

enum SSD1680_Color {
  ColorBlack = 0,
  ColorWhite,
  ColorRed
};

enum SSD1680_Pattern {
  Pattern8 = 0,     /**< 8 pixels */
  Pattern16,        /**< 16 pixels */
  Pattern32,        /**< 32 pixels */
  Pattern64,        /**< 64 pixels */
  Pattern128,       /**< 128 pixels */
  Pattern256,       /**< 256 pixels vertical, solid color horizontal */
  PatternSolid = 7  /**< Solid color */
};

void ssd1680_command(unsigned char command, unsigned char *data, unsigned int data_length); // should be defined in hal.c

void ssd1680_refresh(void);
void ssd1680_write_bw_ram(unsigned char *data, unsigned int length);
void ssd1680_write_red_ram(unsigned char *data, unsigned int length);
void ssd1680_ram_fill(enum SSD1680_Pattern kx, enum SSD1680_Pattern ky, enum SSD1680_Pattern rx,
                      enum SSD1680_Pattern ry, enum SSD1680_Color color);
void ssd1680_fill_screen(enum SSD1680_Color color);
void ssd1680_fill_rect(unsigned short x1, unsigned short dx, unsigned short y1, unsigned short dy, enum SSD1680_Color color);
void ssd1680_set_window(unsigned short x1, unsigned short x2, unsigned short y1, unsigned short y2);
void ssd1680_set_ram_y_counter(unsigned short counter);
void ssd1680_set_ram_x_counter(unsigned char counter);

void ssd1680_init(unsigned char data_entry_mode);

#endif
