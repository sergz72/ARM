#include <board.h>
#include <tsc2046.h>

void delay1(int cnt)
{
  while (cnt--);
}

unsigned short TSC2046_GetAd(unsigned char XY)
{	
  int i;
  unsigned short v;

  TSC_CS_CLR;

  delay1(TSC_DELAY);

  for (i = 0; i < 8; i++)
  {
    if (XY & 0x80)
      TSC_SDO_SET;
    else
      TSC_SDO_CLR;
    delay1(TSC_DELAY);
    TSC_SCK_SET;
    delay1(TSC_DELAY);
    TSC_SCK_CLR;
    XY <<= 1;
  }

  TSC_SDO_CLR;
  delay1(TSC_DELAY);
#ifdef TSC_BUSY
  while (TSC_BUSY)
  {
    TSC_SCK_SET;
    delay1(TSC_DELAY);
    TSC_SCK_CLR;
    delay1(TSC_DELAY);
  }
#else
  TSC_SCK_SET;
  delay1(TSC_DELAY);
  TSC_SCK_CLR;
  delay1(TSC_DELAY);
#endif

  v = 0;

  for (i = 0; i < 16; i++)
  {
    delay1(TSC_DELAY);
    if (i < 12)
    {
      v <<= 1;
      if (TSC_SDI)
        v |= 1;
    }  
    TSC_SCK_SET;
    delay1(TSC_DELAY);
    TSC_SCK_CLR;
  }

  delay1(TSC_DELAY);

  TSC_CS_SET;

  return v;
}

int TSC2046_Read_XY(unsigned char XY)
{
  unsigned short i;
	unsigned short a[12];
	unsigned short temp;
	
	for(i=0;i<=11;i++)
  {
	  if (TSC_PENIRQ)	return -1;   
	  else 	a[i] = TSC2046_GetAd(XY);
	  delay(6);			  
	}

	for(i=0;i<=10;i++)	{		
		if(a[i]>a[i+1]){			
				temp=a[i+1];
				a[i+1]=a[i];
				a[i]=temp;
		}  
	}
		
	temp = (unsigned short)(a[4] + a[5] + a[6] + a[7])/4;
				
	return temp;
}

/*ts_point ts_get_adc_val(void)
{
  ts_point tsp;

  tsp.x = TSC2046_Read_XY(TSC_CMD_RDX);
  tsp.y = TSC2046_Read_XY(TSC_CMD_RDY);

  return tsp;
}*/
