#include <board.h>
#include <st7066.h>

static void st7066_write(unsigned int rs, unsigned int data)
{
	if (rs)
		ST7066_RS_SET;
	else
		ST7066_RS_CLR;
	ST7066_DATA_SET(data);
	ST7066_E_SET;
	ST7066_DELAY;
	ST7066_E_CLR;
	ST7066_DELAY;
}

void st7066_clear_display(unsigned int wait)
{
	st7066_write(0, 1);
	if (wait)
		ST7066_DELAY_MS(2);
}

void st7066_return_home(void)
{
	st7066_write(0, 2);
}

void st7066_entry_mode(unsigned int mode)
{
	st7066_write(0, 4 | mode);
}

void st7066_display_control(unsigned int dcb)
{
	st7066_write(0, 8 | dcb);
}

void st7066_shift(unsigned int scrl)
{
	st7066_write(0, 0x10 | scrl);
}

void st7066_function_set(unsigned int nf)
{
	st7066_write(0, 0x30 | nf);
}

void st7066_set_ddram_address(unsigned int address)
{
	st7066_write(0, 0x80 | (address & 0x7F));
}

void st7066_write_data(unsigned int data)
{
	st7066_write(1, data);
}

void st7066_init(unsigned int num_rows, unsigned int mode)
{
	num_rows = num_rows > 1 ? ST7066_FUNCTION_N : 0;
		
	ST7066_DELAY_MS(15);
	st7066_function_set(num_rows);
	ST7066_DELAY_MS(5);
	st7066_function_set(num_rows);
	ST7066_DELAY_MS(1);
	st7066_function_set(num_rows);
	ST7066_DELAY;
	st7066_function_set(num_rows);
	st7066_display_control(0); // display off
	st7066_clear_display(1);
	st7066_entry_mode(mode);
}
