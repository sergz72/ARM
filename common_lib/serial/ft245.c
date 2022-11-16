#include "board.h"
#include "ft245.h"

void (*_read_handler)(char, int);
void (*_end_handler)(int);

char FT245Read(void)
{
  char c;

  FT245_RD_CLEAR;

  for (c = 0; c <= 5; c++);

  c = FT245_DATA_GET;

  FT245_RD_SET;
  
  return c;
}

void FT245Flush()
{
	while (!FT245_RXF)
  	FT245Read();
}

void FT245_ISR(void)
{
	int count;
  if (IS_FT245_ISR)
  {
    FT245_INTERRUPT_CLEAR;
		count = 0;
  	while (!FT245_RXF)
		{
    	_read_handler(FT245Read(), ++count);
		}
    if (count && _end_handler)
      _end_handler(count);
  }
}

void FT245ConfigureRXInterrupt()
{
   FT245Flush();
   FT245_INTERRUPT_ENABLE;
}

void FT245Init(void (*read_handler)(char, int), void (*end_handler)(int))
{
	 _read_handler = read_handler;
   _end_handler = end_handler;
	 
   FT245_CONFIGURE_IO;
   FT245ConfigureRXInterrupt();
}

void FT245Write(unsigned char c)
{
  FT245_INTERRUPT_DISABLE;

	while (FT245_TXE);
	
  FT245_DATA_OUT(c);
  FT245_WR_SET;
  FT245_WR_CLEAR;

  FT245_DATA_OFF;

  FT245_INTERRUPT_ENABLE;
}
