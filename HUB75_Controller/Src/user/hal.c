#include <board.h>

#define GET_R(v) ((v >> HUB75_COLOR_BITS*2) & HUB75_COLOR_MASK)
#define GET_G(v) ((v >> HUB75_COLOR_BITS) & HUB75_COLOR_MASK)
#define GET_B(v) (v & HUB75_COLOR_MASK)

#define ADDR2_OFFSET HUB75_MODULE_WIDTH * HUB75_NUM_MODULES * HUB75_MODULE_HEIGHT / 2

extern IWDG_HandleTypeDef hiwdg;

extern unsigned short int buffer[];

int row_counter;

static unsigned int get_offset1(unsigned int row_number)
{
	return row_number * HUB75_MODULE_WIDTH * HUB75_NUM_MODULES;
}

static void send_row(unsigned int row_number)
{
	unsigned int i;
  unsigned short int *addr1 = buffer + get_offset1(row_number);
  unsigned short int *addr2 = addr1 + ADDR2_OFFSET;
  unsigned int color_value = (row_counter & HUB75_COLOR_MASK) + 1;
	
	for (i = 0; i < HUB75_MODULE_WIDTH*HUB75_NUM_MODULES; i++)
	{
		unsigned short int v1 = *addr1++;
		unsigned short int v2 = *addr2++;
		unsigned int rgb = 0;

		HUB75_CLK_CLR;
		
		if (GET_R(v1) >= color_value)
			rgb |= HUB75_R1;

		if (GET_G(v1) >= color_value)
			rgb |= HUB75_G1;

		if (GET_B(v1) >= color_value)
			rgb |= HUB75_B1;

		if (GET_R(v2) >= color_value)
			rgb |= HUB75_R2;

		if (GET_G(v2) >= color_value)
			rgb |= HUB75_G2;

		if (GET_B(v2) >= color_value)
			rgb |= HUB75_B2;

    HUB75_RGB_SET(rgb);		
		HUB75_CLK_SET;
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	unsigned int row_number;

  HAL_IWDG_Refresh(&hiwdg);
	
	if (row_counter < 0)
		return;
	
	row_number = row_counter >> HUB75_COLOR_BITS;

  send_row(row_number);
	
  HUB75_OE_SET;
	HUB75_ADDR_SET(row_number);
	HUB75_LAT_SET;
	HUB75_LAT_CLR;
  HUB75_OE_CLR;
	
	row_counter++;
	row_counter &= HUB75_ROW_COUNTER_MASK;
	
/*  HAL_IWDG_Refresh(&hiwdg);

	unsigned int PBV = ((row_counter >> 6) & 7) | ((row_counter << 1) & 0x1C00);

	HUB75_RGB_SET(row_counter & 0x3F);
	GPIOB->ODR = PBV;
	
	row_counter++;*/
}
