#include <board.h>
#include <st7066.h>

unsigned int is_on;
unsigned int volts;
int amp1, amp2;
unsigned int mode, setup_mode;
unsigned int sw_button_pressed, onoff_button_pressed;
unsigned int loop_tick;
volatile int encoder_counter;

#define MODE_FIX 0
#define MODE_SQ 1
#define MODE_MAX 1

#define SETUP_MODE_NONE 0
#define SETUP_MODE_AMP1 1
#define SETUP_MODE_AMP2 2
#define SETUP_MODE_MODE 3
#define SETUP_MODE_MAX  3

#define AMP_MAX 300

#define R2 128300 //120k
#define R1 33000 // 33k
#define VREF 3300 //3.3v
#define RL 300 // 0.3 Ohm

extern ADC_HandleTypeDef hadc;
extern DAC_HandleTypeDef hdac1;

static void update(unsigned int value, unsigned int symbol)
{
	st7066_write_data(((value / 100) % 10) + '0');
	st7066_write_data('.');
	st7066_write_data(((value / 10) % 10) + '0');
	st7066_write_data((value  % 10) + '0');
	st7066_write_data(symbol);
}

static void update_volts(void)
{
	st7066_set_ddram_address(0);
	st7066_write_data(volts < 1000 ? ' ' : (volts / 1000) + '0');
	update(volts, 'V');
}

static void update_amp1(void)
{
	if (mode)
    st7066_display_control(ST7066_DISPLAY_ON);
	st7066_set_ddram_address(0x40);
	update(amp1, 'A');
}

static void update_amp2(void)
{
	if (mode)
    st7066_display_control(ST7066_DISPLAY_ON);
	st7066_set_ddram_address(0x46);
	update(amp2, 'A');
}

static void set_mode(unsigned int m)
{
	if (mode)
    st7066_display_control(ST7066_DISPLAY_ON);
	mode = m;
	st7066_set_ddram_address(7);
	switch (mode)
	{
		case MODE_FIX:
    	st7066_write_data('F');
    	st7066_write_data('I');
    	st7066_write_data('X');
			break;
		case MODE_SQ:
    	st7066_write_data('S');
    	st7066_write_data('Q');
    	st7066_write_data(' ');
			break;
	}
}

void update_setup_mode(void);

static void set_is_on(unsigned int v, unsigned int update_counter)
{
	if (!update_counter && mode)
    st7066_display_control(ST7066_DISPLAY_ON);
	is_on = v;
	st7066_set_ddram_address(11);
	if (is_on)
   	st7066_write_data('1');
	else
   	st7066_write_data('0');
}

static void lcd_init(void)
{
	update_volts();
	update_amp1();
	update_amp2();
	set_mode(0);
	set_is_on(0, 0);
}

static void update_setup_mode(void)
{
	switch (setup_mode)
	{
		case SETUP_MODE_AMP1:
    	st7066_set_ddram_address(0x40);
			st7066_display_control(ST7066_DISPLAY_ON|ST7066_BLINK_ON|ST7066_CURSOR_ON);
		  break;
		case SETUP_MODE_AMP2:
    	st7066_set_ddram_address(0x46);
			st7066_display_control(ST7066_DISPLAY_ON|ST7066_BLINK_ON|ST7066_CURSOR_ON);
		  break;
		case SETUP_MODE_MODE:
    	st7066_set_ddram_address(7);
			st7066_display_control(ST7066_DISPLAY_ON|ST7066_BLINK_ON|ST7066_CURSOR_ON);
		  break;
		default:
			st7066_display_control(ST7066_DISPLAY_ON);
		  break;
	}
}

void set_dac(unsigned int v)
{
	v = v * RL * 4096 / (VREF * 100);
	HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, v);
}

void set_dac_low()
{
	if (!is_on)
	{
		set_dac(0);
		LED_GREEN_OFF;
		LED_RED_OFF;
	}
	else if (mode == MODE_FIX)
	{
		set_dac(amp2);
		LED_GREEN_OFF;
		LED_RED_ON;
	}
	else
	{
		set_dac(amp1);
		LED_GREEN_ON;
		LED_RED_OFF;
	}
}

void set_dac_high()
{
	if (!is_on)
	{
		set_dac(0);
		LED_GREEN_OFF;
		LED_RED_OFF;
	}
	else
	{
		set_dac(amp2);
		LED_RED_ON;
		LED_GREEN_OFF;
	}
}

void user_init(void)
{
	volts = setup_mode = sw_button_pressed = onoff_button_pressed = loop_tick = 0;
	amp1 = amp2 = encoder_counter = 0;
	
	HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
	set_dac(0);
	
	st7066_init(2, ST7066_ENTRY_MODE_ADDRESS_INCREMENT);
	lcd_init();
  st7066_display_control(ST7066_DISPLAY_ON);
}

void user_loop(void)
{
	int counter;
	int update_counter = 0;
	
	__disable_irq();
	counter = encoder_counter;
	encoder_counter = 0;
	__enable_irq();
	
	if (counter)
	{
		switch (setup_mode)
		{
			case SETUP_MODE_AMP1:
				counter = amp1 + counter * 10;
			  if (counter < 0)
					counter = 0;
				else if (counter > amp2)
					counter = amp2;
				if (counter != amp1)
				{
					amp1 = counter;
				  update_amp1();
					update_counter++;
				}
				break;
			case SETUP_MODE_AMP2:
				counter = amp2 + counter * 10;
			  if (counter < amp1)
					counter = amp1;
				else if (counter > AMP_MAX)
					counter = AMP_MAX;
				if (counter != amp2)
				{
					amp2 = counter;
				  update_amp2();
					update_counter++;
				}
				break;
			case SETUP_MODE_MODE:
				set_mode((mode + counter) % (MODE_MAX + 1));
			  update_counter++;
				break;
		}
	}
		
	if (SW_BUTTON_PRESSED)
	{
		sw_button_pressed++;
		if (sw_button_pressed == 3)
		{
			if (setup_mode < SETUP_MODE_MAX)
				setup_mode++;
			else
				setup_mode = 0;
			update_counter++;
		}
	}
	else
		sw_button_pressed = 0;

	if (ONOFF_BUTTON_PRESSED)
	{
		onoff_button_pressed++;
		if (onoff_button_pressed == 3)
		{
			set_is_on(is_on ? 0 : 1, update_counter++);
		}
	}
	else
		onoff_button_pressed = 0;

	counter = loop_tick % 10;
	if (counter == 8)
    HAL_ADC_Start(&hadc);
	else if (counter == 9)
	{
	  if (HAL_ADC_PollForConversion(&hadc, 0xFFFFFFFF) == HAL_OK)
		{
			counter = HAL_ADC_GetValue(&hadc);
			volts = counter * VREF / 4096 * (R1 + R2) / (R1 * 10);
			update_volts();
			update_counter++;
		}
	}
	
	counter = loop_tick % 20;
	if (!counter)
		set_dac_low();
	else if (counter == 10)
		set_dac_high();
	
	loop_tick++;

  if (update_counter)
		update_setup_mode();
		
	HAL_Delay(50);
}
