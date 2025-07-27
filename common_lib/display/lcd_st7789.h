#ifndef LCD_ST7789_H
#define LCD_ST7789_H

#define	BLUE_COLOR    0x1F00
#define	RED_COLOR     0x00F8
#define	GREEN_COLOR   0xE007
#define CYAN_COLOR    0xFF7F
#define MAGENTA_COLOR 0x1FF8
#define YELLOW_COLOR  0xE0FF
#define WHITE_COLOR   0xFFFF
#define GRAY_COLOR    0x3084
#define RGB(r, g, b) ((r & 0xF8) | ((g >> 5) & 7) | ((g & 0x1C) << 11) | ((b & 0xF8) << 5))

// MADCTL Parameters
#define ST7789_MADCTL_MH  0x04  // Bit 2 - Refresh Left to Right
#define ST7789_MADCTL_RGB 0x00  // Bit 3 - RGB Order
#define ST7789_MADCTL_BGR 0x08  // Bit 3 - BGR Order
#define ST7789_MADCTL_ML  0x10  // Bit 4 - Scan Address Increase
#define ST7789_MADCTL_MV  0x20  // Bit 5 - X-Y Exchange
#define ST7789_MADCTL_MX  0x40  // Bit 6 - X-Mirror
#define ST7789_MADCTL_MY  0x80  // Bit 7 - Y-Mirror

#endif
