#include <board.h>
#include "usbd_cdc_if.h"

#define DISPLAY_OFF 0
#define DISPLAY_ON 1
#define CLEAR_SCREEN 2
#define FILL 3
#define TEXT 4

typedef struct
{
	unsigned char id;
	unsigned char length;
	unsigned short color;
  unsigned char x, y;
  union
	{
		struct
		{
			unsigned char w, h;
		} WH;
		struct
		{
		  unsigned char font_id;
			unsigned char text_length;
			char text[DRAW_TEXT_MAX];
		} Text;
	} Parameters;
} Command;

extern TIM_HandleTypeDef htim14;
volatile Command cmd;

void error(const char *message)
{
	CDC_Transmit_FS((unsigned char*)message, 4);
}

void ok(void)
{
	CDC_Transmit_FS((unsigned char*)"done", 4);
}

void serial_receive(unsigned char *buf, int len)
{
	Command *c;
	
	if (cmd.length)
	{
		error("e001");
		return;
	}
	
	if (len < 2 || len > 64)
	{
		error("e002");
		return;
	}
	
	c = (Command*)buf;
	if (c->length != len)
	{
		error("e003");
		return;
	}
	
	switch (c->id)
	{
		case DISPLAY_OFF:
		case DISPLAY_ON:
		case CLEAR_SCREEN:
			if (len != 2)
			{
				error("e004");
				return;
			}
			break;
		case FILL:
			if (len != 8)
			{
				error("e005");
				return;
			}
			break;
		case TEXT:
			if (c->Parameters.Text.text_length > DRAW_TEXT_MAX || len != 8 + c->Parameters.Text.text_length)
			{
				error("e006");
				return;
			}
			break;
		default:
  		error("e007");
		  return;
	}
	memcpy((void*)&cmd, buf, len);
}

void user_init(void)
{
	cmd.length = 0;
	
	display_off();
	clear_screen();
//	fill(1, 1, 2, 2, 0x100);//R
//	fill(3, 1, 2, 2, 0x10);//G
//	fill(5, 1, 2, 2, 1);//B
	
	__HAL_RCC_DBGMCU_CLK_ENABLE();
	__HAL_DBGMCU_FREEZE_IWDG();
	TIM14->DIER = TIM_DIER_UIE;
	HAL_TIM_Base_Start(&htim14);
}

void user_loop(void)
{
	if (cmd.length)
	{
		switch (cmd.id)
		{
			case DISPLAY_OFF:
				display_off();
			  break;
			case DISPLAY_ON:
				display_on();
			  break;
			case CLEAR_SCREEN:
				clear_screen();
				break;
			case FILL:
				fill(cmd.x, cmd.y, cmd.Parameters.WH.w, cmd.Parameters.WH.h, cmd.color);
				break;
			case TEXT:
				if (!text(cmd.x, cmd.y, cmd.color, cmd.Parameters.Text.font_id, cmd.Parameters.Text.text_length, (char*)cmd.Parameters.Text.text))
				{
					error("e008");
					cmd.length = 0;
					return;
				}
				break;
		}
		ok();
		cmd.length = 0;
	}
}
