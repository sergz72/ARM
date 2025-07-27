#ifndef LCD_SSD1331_H
#define LCD_SSD1331_H

#define	BLUE_COLOR    0x1F00
#define	RED_COLOR     0x00F8
#define	GREEN_COLOR   0xE007
#define CYAN_COLOR    0xFF7F
#define MAGENTA_COLOR 0x1FF8
#define YELLOW_COLOR  0xE0FF
#define WHITE_COLOR   0xFFFF
#define GRAY_COLOR    0x3084
#define RGB(r, g, b) ((r & 0xF8) | ((g >> 5) & 7) | ((g & 0x1C) << 11) | ((b & 0xF8) << 5))

#define SSD1331_MADCTL_XY_SWAP  1
#define SSD1331_MADCTL_X_MIRROR 2
#define SSD1331_MADCTL_Y_MIRROR 0x10

#endif
