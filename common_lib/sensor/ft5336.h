#ifndef _FT5336_H
#define _FT5336_H

#include "ts_point.h"

#define FT5336_SWAP_NONE 1U
#define FT5336_SWAP_X    2U
#define FT5336_SWAP_Y    4U
#define FT5336_SWAP_XY   8U

unsigned int FT5336_Init(unsigned int ts_SizeX, unsigned int ts_SizeY, unsigned int orientation);
void FT5336_GetState(ts_point *p);
unsigned char FT5336_Read(unsigned short reg_no);
unsigned int FT5336_Write(unsigned short reg_no, unsigned char value);

#endif
