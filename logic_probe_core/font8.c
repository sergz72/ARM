#include <spi_lcd_common.h>
#include <font8.h>

static void draw_symbol_30(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_31(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_32(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_33(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_34(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_35(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_36(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_37(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_38(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_39(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_25(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_48(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_4c(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_55(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_56(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_5a(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_20(unsigned int text_color, unsigned int bk_color);

static void draw_symbol_300(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_310(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_320(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_330(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_340(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_350(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_360(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_370(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_380(unsigned int text_color, unsigned int bk_color);
static void draw_symbol_390(unsigned int text_color, unsigned int bk_color);

const draw_symbol font8_symbols[27] = {
  draw_symbol_30,
  draw_symbol_31,
  draw_symbol_32,
  draw_symbol_33,
  draw_symbol_34,
  draw_symbol_35,
  draw_symbol_36,
  draw_symbol_37,
  draw_symbol_38,
  draw_symbol_39,
  draw_symbol_300,
  draw_symbol_310,
  draw_symbol_320,
  draw_symbol_330,
  draw_symbol_340,
  draw_symbol_350,
  draw_symbol_360,
  draw_symbol_370,
  draw_symbol_380,
  draw_symbol_390,
  draw_symbol_25,
  draw_symbol_48,
  draw_symbol_4c,
  draw_symbol_55,
  draw_symbol_56,
  draw_symbol_5a,
  draw_symbol_20
};

//0
static void draw_symbol_30(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 11);
}

//0.
static void draw_symbol_300(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 8);
}

//1
static void draw_symbol_31(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 2);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 5);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 10);
}

//1.
static void draw_symbol_310(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 2);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 5);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 8);
}

//2
static void draw_symbol_32(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 10);
}

//2.
static void draw_symbol_320(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 8);
}

//3
static void draw_symbol_33(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 5);
  Lcd_WriteColor(text_color, 2);
  Lcd_WriteColor(bk_color, 8);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 11);
}

//3.
static void draw_symbol_330(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 5);
  Lcd_WriteColor(text_color, 2);
  Lcd_WriteColor(bk_color, 8);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 8);
}

//4
static void draw_symbol_34(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 2);
  Lcd_WriteColor(bk_color, 5);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 10);
}

//4.
static void draw_symbol_340(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 2);
  Lcd_WriteColor(bk_color, 5);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 8);
}

//5
static void draw_symbol_35(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 4);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 8);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 11);
}

//5.
static void draw_symbol_350(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 4);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 8);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 8);
}

//6
static void draw_symbol_36(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 4);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 11);
}

//6.
static void draw_symbol_360(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 4);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 8);
}

//7
static void draw_symbol_37(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 12);
}

//7
static void draw_symbol_370(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 8);
}

//8
static void draw_symbol_38(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 11);
}

//8.
static void draw_symbol_380(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 8);
}

//9
static void draw_symbol_39(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 4);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 12);
}

//9.
static void draw_symbol_390(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 4);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 8);
}

//%
static void draw_symbol_25(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 4);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 12);
}

//H
static void draw_symbol_48(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 9);
}

//L
static void draw_symbol_4c(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 7);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 10);
}

//U
static void draw_symbol_55(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 11);
}

//V
static void draw_symbol_56(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 3);
  Lcd_WriteColor(bk_color, 2);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 4);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 12);
}

//Z
static void draw_symbol_5a(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 1);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 6);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 1);
  Lcd_WriteColor(bk_color, 3);
  Lcd_WriteColor(text_color, 5);
  Lcd_WriteColor(bk_color, 10);
}

//
static void draw_symbol_20(unsigned int text_color, unsigned int bk_color)
{
  Lcd_WriteColor(bk_color, 64);
}
