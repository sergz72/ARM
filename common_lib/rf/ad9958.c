#include <ad9958.h>
#include <board.h>

void AD9958_Init(void)
{
  AD9958_CONFIGURE_IO;
	delay(1000);
	AD9958_PWRDWN_CLR;
	delay(1000);
	AD9958_RESET_CLR;
}

void AD9958_Set(unsigned int channel, unsigned int ftw, unsigned short asf)
{
	switch (channel)
	{
	  case 0:
			AD9958_Write8(AD9958_CSR_REGISTER, 0x40); // Channel 0 is selected
		  break;
	  case 1:
			AD9958_Write8(AD9958_CSR_REGISTER, 0x80); // Channel 1 is selected
		  break;
		default:
			return;
	}

	AD9958_Write32(AD9958_CFTW0_REGISTER, ftw);
	AD9958_Write24(AD9958_ACR_REGISTER, (asf & 0x3FF) | 0x1000);
}

void AD9958_Write_Data8(unsigned char data)
{
	int i;
	
	for (i = 0; i < 8; i++)
	{
		if (data & 0x80)
			AD9958_SDIO_SET;
		else
			AD9958_SDIO_CLR;
		AD9958_SCLK_SET;
		data <<= 1;
		AD9958_SCLK_CLR;
	}
}

void AD9958_Write8(unsigned char instruction, unsigned char data)
{
	AD9958_IOUPDATE_CLR;
	AD9958_CS_CLR;
  AD9958_Write_Data8(instruction & 0x7F);
  AD9958_Write_Data8(data);
	AD9958_CS_SET;
	AD9958_IOUPDATE_SET;
}

void AD9958_Write_Data16(unsigned short data)
{
	int i;
	
	for (i = 0; i < 16; i++)
	{
		if (data & 0x8000)
			AD9958_SDIO_SET;
		else
			AD9958_SDIO_CLR;
		AD9958_SCLK_SET;
		data <<= 1;
		AD9958_SCLK_CLR;
	}
}


void AD9958_Write16(unsigned char instruction, unsigned short data)
{
	AD9958_IOUPDATE_CLR;
	AD9958_CS_CLR;
  AD9958_Write_Data8(instruction & 0x7F);
  AD9958_Write_Data16(data);
	AD9958_CS_SET;
	AD9958_IOUPDATE_SET;
}

void AD9958_Write_Data24(unsigned int data)
{
	int i;
	
	for (i = 0; i < 24; i++)
	{
		if (data & 0x800000)
			AD9958_SDIO_SET;
		else
			AD9958_SDIO_CLR;
		AD9958_SCLK_SET;
		data <<= 1;
		AD9958_SCLK_CLR;
	}
}


void AD9958_Write24(unsigned char instruction, unsigned int data)
{
	AD9958_IOUPDATE_CLR;
	AD9958_CS_CLR;
  AD9958_Write_Data8(instruction & 0x7F);
  AD9958_Write_Data24(data);
	AD9958_CS_SET;
	AD9958_IOUPDATE_SET;
}

void AD9958_Write_Data32(unsigned int data)
{
	int i;
	
	for (i = 0; i < 32; i++)
	{
		if (data & 0x80000000)
			AD9958_SDIO_SET;
		else
			AD9958_SDIO_CLR;
		AD9958_SCLK_SET;
		data <<= 1;
		AD9958_SCLK_CLR;
	}
}


void AD9958_Write32(unsigned char instruction, unsigned int data)
{
	AD9958_IOUPDATE_CLR;
	AD9958_CS_CLR;
  AD9958_Write_Data8(instruction & 0x7F);
  AD9958_Write_Data32(data);
	AD9958_CS_SET;
	AD9958_IOUPDATE_SET;
}

void AD9958_Write_Data2(unsigned char data)
{
	int i;
	
	for (i = 0; i < 8; i++)
	{
		if (data & 0x80)
			AD9958_SDIO_SET;
		else
			AD9958_SDIO_CLR;
		AD9958_SCLK_CLR;
		data <<= 1;
		AD9958_SCLK_SET;
	}
	AD9958_SDIO_MODE_IN;
}

unsigned char AD9958_Read_Data8(void)
{
	int i;
	unsigned char data;
	
	data = 0;
	
	for (i = 0; i < 8; i++)
	{
		AD9958_SCLK_CLR;
		AD9958_SCLK_CLR;
		data <<= 1;
		if (AD9958_SDIO_GET)
			data |= 1;
		AD9958_SCLK_SET;
	}
	
	return data;
}

unsigned char AD9958_Read8(unsigned char instruction)
{
	unsigned char data;
	
	AD9958_SCLK_SET;
	AD9958_CS_CLR;
  AD9958_Write_Data2(instruction | 0x80);
  data = AD9958_Read_Data8();
	AD9958_CS_SET;
	AD9958_SDIO_MODE_OUT;
	AD9958_SCLK_CLR;
	return data;
}

unsigned short AD9958_Read_Data16(void)
{
	int i;
	unsigned short data;
	
	data = 0;
	
	for (i = 0; i < 16; i++)
	{
		AD9958_SCLK_CLR;
		AD9958_SCLK_CLR;
		data <<= 1;
		if (AD9958_SDIO_GET)
			data |= 1;
		AD9958_SCLK_SET;
	}
	
	return data;
}

unsigned short AD9958_Read16(unsigned char instruction)
{
	unsigned short data;
	
	AD9958_SCLK_SET;
	AD9958_CS_CLR;
  AD9958_Write_Data2(instruction | 0x80);
  data = AD9958_Read_Data16();
	AD9958_CS_SET;
	AD9958_SDIO_MODE_OUT;
	AD9958_SCLK_CLR;
	return data;
}

unsigned int AD9958_Read_Data(unsigned int n)
{
	int i;
	unsigned int data;
	
	data = 0;
	
	for (i = 0; i < n; i++)
	{
		AD9958_SCLK_CLR;
		AD9958_SCLK_CLR;
		data <<= 1;
		if (AD9958_SDIO_GET)
			data |= 1;
		AD9958_SCLK_SET;
	}
	
	return data;
}

unsigned int AD9958_Read24(unsigned char instruction)
{
	unsigned int data;
	
	AD9958_SCLK_SET;
	AD9958_CS_CLR;
  AD9958_Write_Data2(instruction | 0x80);
  data = AD9958_Read_Data(24);
	AD9958_CS_SET;
	AD9958_SDIO_MODE_OUT;
	AD9958_SCLK_CLR;
	return data;
}

unsigned int AD9958_Read32(unsigned char instruction)
{
	unsigned int data;
	
	AD9958_SCLK_SET;
	AD9958_CS_CLR;
  AD9958_Write_Data2(instruction | 0x80);
  data = AD9958_Read_Data(32);
	AD9958_CS_SET;
	AD9958_SDIO_MODE_OUT;
	AD9958_SCLK_CLR;
	return data;
}
