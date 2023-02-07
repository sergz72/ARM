#ifndef _LCD_RGB_H
#define _LCD_RGB_H

/** 
  * @brief  LCD Direction  
  */ 
#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001

/** 
  * @brief  LCD Layer  
  */ 
#define LCD_BACKGROUND_LAYER     0x0000
#define LCD_FOREGROUND_LAYER     0x0001

/** 
  * @brief  LCD color  
  */ 
#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

typedef struct 
{
  int16_t X;
  int16_t Y;
} Point, * pPoint;   

void LCD_Init(void);
void LCD_LayerInit(void);
void LCD_SetLayer(uint32_t Layerx);
void LCD_Clear(uint16_t Color);
void LCD_SetTransparency(uint8_t transparency);
void LCD_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width);
void LCD_WindowModeDisable(void);
void LCD_DrawLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction, uint16_t color);
void LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width, uint16_t color);
void LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius, uint16_t color);
void LCD_DrawFullEllipse(int Xpos, int Ypos, int Radius, int Radius2, uint16_t color);
void LCD_DrawEllipse(int Xpos, int Ypos, int Radius, int Radius2, uint16_t color);
void LCD_DrawUniLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, int16_t color);
void LCD_DrawFullRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t color);
void LCD_DrawFullCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius, uint16_t color);
void LCD_PolyLine(pPoint Points, uint16_t PointCount, uint16_t color);
void LCD_PolyLineRelative(pPoint Points, uint16_t PointCount, uint16_t color);
void LCD_ClosedPolyLine(pPoint Points, uint16_t PointCount, uint16_t color);
void LCD_ClosedPolyLineRelative(pPoint Points, uint16_t PointCount, uint16_t color);
void LCD_FillPolyLine(pPoint Points, uint16_t PointCount, uint16_t color);
void LCD_Triangle(pPoint Points, uint16_t PointCount, uint16_t color);
void LCD_FillTriangle(uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3, uint16_t color);
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_ReSetColorKeying(void);
void LCD_SetColorKeying(uint32_t RGBValue);

extern uint32_t CurrentFrameBuffer;

#endif
