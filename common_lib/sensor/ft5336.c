#include <board.h>
#include "ft5336.h"

/* FT5336 Chip identification register */
#define FT5336_CHIP_ID_REG 0xA8U

#define FT5336_ID_VALUE 0x51U

/* Interrupt mode register (used when in interrupt mode) */
#define FT5336_GMODE_REG 0xA4U

/* Touch Data Status register : gives number of active touch points (0..5) */
#define FT5336_TD_STAT_REG 0x02U

/* Values related to FT5336_TD_STAT_REG */
#define FT5336_TD_STAT_MASK 0x0FU

#define FT5336_P1_XH_REG 3U
#define FT5336_P1_XL_REG 4U
#define FT5336_P1_YH_REG 5U
#define FT5336_P1_YL_REG 6U

/* Values Pn_XH and Pn_YH related */
#define FT5336_TOUCH_POS_MSB_MASK 0x0F

/* Values Pn_XL and Pn_YL related */
#define FT5336_TOUCH_POS_LSB_MASK 0xFF

static unsigned int tsXBoundary, tsYBoundary, tsOrientation;

static unsigned int FT5336_Detect(void)
{
  uint8_t nbReadAttempts;

  /* At maximum 4 attempts to read ID : exit at first finding of the searched device ID */
  for(nbReadAttempts = 0; nbReadAttempts < 3; nbReadAttempts++)
  {
    /* Read register FT5336_CHIP_ID_REG as DeviceID detection */
    if(FT5336_Read(FT5336_CHIP_ID_REG) == FT5336_ID_VALUE)
      return 1;
  }

  return 0;
}

static void FT5336_DisableIT()
{
  FT5336_Write(FT5336_GMODE_REG, 0);
}

static unsigned int FT5336_DetectTouch(void)
{
  /* Read register FT5336_TD_STAT_REG to check number of touches detection */
  return (FT5336_Read(FT5336_TD_STAT_REG) & FT5336_TD_STAT_MASK) == 1;
}

unsigned int FT5336_Init(unsigned int ts_SizeX, unsigned int ts_SizeY, unsigned int orientation)
{
  tsXBoundary = ts_SizeX;
  tsYBoundary = ts_SizeY;
  tsOrientation = orientation;

  /* Wait at least 200ms after power up before accessing registers
   * Trsi timing (Time of starting to report point after resetting) from FT5336GQQ datasheet */
  delayms(200);
  /* Read ID and verify if the touch screen driver is ready */
  if(FT5336_Detect())
  {
    FT5336_DisableIT();

    return 0;
  }

  return 1;
}

static void FT5336_GetXY(int *x, int *y)
{
  int ucReadData, coord;

  /* Read low part of X position */
  ucReadData = FT5336_Read(FT5336_P1_XL_REG);
  coord = ucReadData & FT5336_TOUCH_POS_LSB_MASK;

  /* Read high part of X position */
  ucReadData = FT5336_Read(FT5336_P1_XH_REG);
  coord |= (ucReadData & FT5336_TOUCH_POS_MSB_MASK) << 8;

  /* Send back ready X position to caller */
  *x = coord;

  /* Read low part of Y position */
  ucReadData = FT5336_Read(FT5336_P1_YL_REG);
  coord = ucReadData & FT5336_TOUCH_POS_LSB_MASK;

  /* Read high part of Y position */
  ucReadData = FT5336_Read(FT5336_P1_YH_REG);
  coord |= (ucReadData & FT5336_TOUCH_POS_MSB_MASK) << 8;

  /* Send back ready Y position to caller */
  *y = coord;
}

void FT5336_GetState(ts_point *p)
{
  int x, y, tmp;

  if (FT5336_DetectTouch())
  {
    /* Get each touch coordinates */
    FT5336_GetXY(&x, &y);

    if(tsOrientation & FT5336_SWAP_X)
    {
      x = 4096 - x;
    }

    if(tsOrientation & FT5336_SWAP_Y)
    {
      y = 4096 - y;
    }

    if(tsOrientation & FT5336_SWAP_XY)
    {
      tmp = x;
      x = y;
      y = tmp;
    }

    p->x = x; p->y = y;
    return;
  }
  p->x = p->y = -1;
}
