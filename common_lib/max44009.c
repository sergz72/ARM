#include <board.h>
#include <max44009.h>

int max44009GetLuxH(unsigned short device_address, unsigned int *result)
{
	unsigned char luxh, luxl;
  unsigned int lux;
	
	if (max44009_read(device_address, MAX44009_REG_LUXH, &luxh) || max44009_read(device_address, MAX44009_REG_LUXL, &luxl))
		return 1;
	luxl = ((luxh & 0x0F) << 4) + (luxl & 0x0F);
	lux = (1 << (luxh >>= 4)) * 45;
	*result = lux * luxl;
	return 0;
}

int max44009GetLuxL(unsigned short device_address, unsigned int *result)
{
	unsigned char luxh;
  unsigned int lux, luxl;
	
	if (max44009_read(device_address, MAX44009_REG_LUXH, &luxh))
		return 1;
	luxl = luxh & 0x0F;
	lux = (1 << (luxh >>= 4)) * 720;
	*result = lux * luxl;
	return 0;
}
