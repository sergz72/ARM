#include "board.h"
#include "ui.h"
#include <lcd.h>
#include <font14.h>

/*
LCD layout

C1kHz
250.0
PWM %
50.00
C2kHz
250.0
PWM %
50.00

*/

#define CURSOR_CH     0
#define CURSOR_HZ     1
#define CURSOR_FREQ0  2
#define CURSOR_FREQ1  3
#define CURSOR_FREQ2  4
#define CURSOR_FREQ3  5
#define CURSOR_MODE   6
#define CURSOR_PARAM0 7
#define CURSOR_PARAM1 8
#define CURSOR_PARAM2 9
#define CURSOR_PARAM3 10
#define CURSOR_POSITION_MAX 10

#define CURSOR_TIMEOUT 20

unsigned int frequency[2];
unsigned int duty[2];
unsigned int amplitude[2];

static unsigned int cursorPosition, cursorEnabled;
static unsigned int outputEnabled[2], currentChannel, mode[2], frequencyMultiplier[2];

static void DrawChar(unsigned int col, unsigned int y, char c, const FONT_INFO* f, unsigned int textColor, unsigned int bkColor, unsigned int swapColors)
{
  char text[2];
  text[0] = c;
  text[1] = 0;
  if (swapColors)
    LcdDrawText(col * (f->character_max_width + f->character_spacing), y, text, f, bkColor, textColor, NULL);
  else
    LcdDrawText(col * (f->character_max_width + f->character_spacing), y, text, f, textColor, bkColor, NULL);
}

static void DrawText(unsigned int col, unsigned int y, char *text, const FONT_INFO* f, unsigned int textColor, unsigned int bkColor, unsigned int swapColors)
{
  if (swapColors)
    LcdDrawText(col * (f->character_max_width + f->character_spacing), y, text, f, bkColor, textColor, NULL);
  else
    LcdDrawText(col * (f->character_max_width + f->character_spacing), y, text, f, textColor, bkColor, NULL);
}

static void DrawFREQ0(unsigned int channel)
{
  int swapColors = cursorEnabled && channel == currentChannel && cursorPosition == CURSOR_FREQ0;
  DrawChar(0, 20 + channel * 80, '0' + frequency[channel] / 1000, &courierNew14ptFontInfo, CYAN_COLOR, BLACK_COLOR, swapColors);
}

static void DrawFREQ1(unsigned int channel)
{
  int swapColors = cursorEnabled && channel == currentChannel && cursorPosition == CURSOR_FREQ1;
  DrawChar(1, 20 + channel * 80, '0' + ((frequency[channel] / 100) % 10), &courierNew14ptFontInfo, CYAN_COLOR, BLACK_COLOR, swapColors);
}

static void DrawFREQ2(unsigned int channel)
{
  int swapColors = cursorEnabled && channel == currentChannel && cursorPosition == CURSOR_FREQ2;
  DrawChar(2, 20 + channel * 80, '0' + ((frequency[channel] / 10) % 10), &courierNew14ptFontInfo, CYAN_COLOR, BLACK_COLOR, swapColors);
}

static void DrawFREQ3(unsigned int channel)
{
  int swapColors = cursorEnabled && channel == currentChannel && cursorPosition == CURSOR_FREQ3;
  DrawChar(4, 20 + channel * 80, '0' + (frequency[channel] % 10), &courierNew14ptFontInfo, CYAN_COLOR, BLACK_COLOR, swapColors);
}

static void DrawFREQ(unsigned int channel)
{
  DrawFREQ0(channel);
  DrawFREQ1(channel);
  DrawFREQ2(channel);
  DrawFREQ3(channel);
}

static void DrawPARAM0(unsigned int channel)
{
  int swapColors = cursorEnabled && channel == currentChannel && cursorPosition == CURSOR_PARAM0;
  unsigned int v = mode[channel] == MODE_PWM ? duty[channel] : amplitude[channel];
  DrawChar(0, 60 + channel * 80, '0' + v / 1000, &courierNew14ptFontInfo, CYAN_COLOR, BLACK_COLOR, swapColors);
}

static void DrawPARAM1(unsigned int channel)
{
  int swapColors = cursorEnabled && channel == currentChannel && cursorPosition == CURSOR_PARAM1;
  unsigned int v = mode[channel] == MODE_PWM ? duty[channel] : amplitude[channel];
  DrawChar(1, 60 + channel * 80, '0' + ((v / 100) % 10), &courierNew14ptFontInfo, CYAN_COLOR, BLACK_COLOR, swapColors);
}

static void DrawPARAM2(unsigned int channel)
{
  int swapColors = cursorEnabled && channel == currentChannel && cursorPosition == CURSOR_PARAM2;
  unsigned int v = mode[channel] == MODE_PWM ? duty[channel] : amplitude[channel];
  DrawChar(3, 60 + channel * 80, '0' + ((v / 10) % 10), &courierNew14ptFontInfo, CYAN_COLOR, BLACK_COLOR, swapColors);
}

static void DrawPARAM3(unsigned int channel)
{
  int swapColors = cursorEnabled && channel == currentChannel && cursorPosition == CURSOR_PARAM3;
  unsigned int v = mode[channel] == MODE_PWM ? duty[channel] : amplitude[channel];
  DrawChar(4, 60 + channel * 80, '0' + (v % 10), &courierNew14ptFontInfo, CYAN_COLOR, BLACK_COLOR, swapColors);
}

static void DrawPARAM(unsigned int channel)
{
  DrawPARAM0(channel);
  DrawPARAM1(channel);
  DrawPARAM2(channel);
  DrawPARAM3(channel);
}

static void DrawCH(unsigned int channel)
{
  unsigned int color = outputEnabled[channel] ? GREEN_COLOR : GRAY_COLOR;
  int swapColors = cursorEnabled && channel == currentChannel && cursorPosition == CURSOR_CH;
  char c = '0' + channel;
  channel *= 80;
  DrawChar(0, channel, 'C', &courierNew14ptFontInfo, color, BLACK_COLOR, swapColors);
  DrawChar(1, channel, c, &courierNew14ptFontInfo, color, BLACK_COLOR, swapColors);
}

static void DrawHZ(unsigned int channel)
{
  int swapColors = cursorEnabled && channel == currentChannel && cursorPosition == CURSOR_HZ;
  DrawChar(2, channel * 80, frequencyMultiplier[channel] == 1 ? ' ' : 'k', &courierNew14ptFontInfo, BLUE_COLOR, BLACK_COLOR, swapColors);
}

static void SetParams(void)
{
  setParams(currentChannel, mode[currentChannel], frequency[currentChannel] * frequencyMultiplier[currentChannel], duty[currentChannel], amplitude[currentChannel] * 10);
}

static void ToggleOutput(void)
{
  unsigned int enabled = ~outputEnabled[currentChannel];
  outputEnabled[currentChannel] = enabled;
  if (enabled)
    SetParams();
  enableOutput(mode[currentChannel], currentChannel, enabled);
  DrawCH(currentChannel);
}

static void DrawModeText(char* text, unsigned int channel)
{
  int swapColors = cursorEnabled && channel == currentChannel && cursorPosition == CURSOR_MODE;
  DrawText(0, 40 + channel * 80, text, &courierNew14ptFontInfo, YELLOW_COLOR, BLACK_COLOR, swapColors);
}

static void DrawMode(unsigned int channel)
{
  switch (mode[channel])
  {
  case MODE_PWM:
    DrawModeText("PWM %", channel);
    break;
  case MODE_SINE:
    DrawModeText("SIN V", channel);
    break;
  case MODE_SQUARE:
    DrawModeText("SQR V", channel);
    break;
  case MODE_TRIANGLE:
    DrawModeText("TRI V", channel);
    break;
  }
}

static void SetNextMode(void)
{
  unsigned int m = mode[currentChannel];
  mode[currentChannel] = m == MODE_MAX ? 0 : m + 1;
  DrawMode(currentChannel);
  if (outputEnabled[currentChannel])
    SetParams();
}

static void SetPrevMode(void)
{
  unsigned int m = mode[currentChannel];
  mode[currentChannel] = m == 0 ? MODE_MAX : m - 1;
  DrawMode(currentChannel);
  if (outputEnabled[currentChannel])
    SetParams();
}

static void SetNextMultiplier(void)
{
  if (frequencyMultiplier[currentChannel] == 1)
    frequencyMultiplier[currentChannel] = 1000;
  else
    frequencyMultiplier[currentChannel] = 1;
  DrawHZ(currentChannel);
  if (outputEnabled[currentChannel])
    SetParams();
}

static void updateSETValues(unsigned int up)
{
  unsigned int f = frequency[currentChannel];
  unsigned int p = mode[currentChannel] == MODE_PWM ? duty[currentChannel] : amplitude[currentChannel];
  unsigned int deltaf = 0, deltap = 0;
  switch (cursorPosition)
  {
  case CURSOR_CH:
    ToggleOutput();
    return;
  case CURSOR_HZ:
    SetNextMultiplier();
    return;
  case CURSOR_MODE:
    if (up)
      SetNextMode();
    else
      SetPrevMode();
    return;
  case CURSOR_FREQ0:
    deltaf = 1000;
    deltap = 0;
    break;
  case CURSOR_FREQ1:
    deltaf = 100;
    deltap = 0;
    break;
  case CURSOR_FREQ2:
    deltaf = 10;
    deltap = 0;
    break;
  case CURSOR_FREQ3:
    deltaf = 1;
    deltap = 0;
    break;
  case CURSOR_PARAM0:
    deltaf = 0;
    deltap = 1000;
    break;
  case CURSOR_PARAM1:
    deltaf = 0;
    deltap = 100;
    break;
  case CURSOR_PARAM2:
    deltaf = 0;
    deltap = 10;
    break;
  case CURSOR_PARAM3:
    deltaf = 0;
    deltap = 1;
    break;
  }
  if (up)
  {
    f += deltaf;
    p += deltap;
  }
  else
  {
    f -= deltaf;
    p -= deltap;
  }
  if (f >= FMIN && f <= FMAX)
    frequency[currentChannel] = f;
  if (mode[currentChannel] == MODE_PWM)
  {
    if (p >= DMIN && p <= DMAX)
      duty[currentChannel] = p;
  }
  else
  {
    if (p >= AMIN && p <= AMAX)
      amplitude[currentChannel] = p;
  }
  if (outputEnabled[currentChannel])
    SetParams();
}

static void DrawSETS(unsigned int channel)
{
  DrawCH(channel);
  DrawHZ(channel);
  DrawFREQ(channel);
  DrawMode(channel);
  DrawPARAM(channel);
}

static void DrawChannel(unsigned int channel)
{
  DrawText(3, channel * 80, "Hz", &courierNew14ptFontInfo, BLUE_COLOR, BLACK_COLOR, FALSE);
  DrawChar(3, 20 + channel * 80, '.', &courierNew14ptFontInfo, CYAN_COLOR, BLACK_COLOR, FALSE);
  DrawChar(2, 60 + channel * 80, '.', &courierNew14ptFontInfo, CYAN_COLOR, BLACK_COLOR, FALSE);
  DrawSETS(channel);
}

void UI_Init(void)
{
  LcdInit();

  cursorPosition = CURSOR_CH;
  currentChannel = 0;
  outputEnabled[0] = outputEnabled[1] = cursorEnabled = FALSE;
  frequency[0] = frequency[1] = 1000;
  frequencyMultiplier[0] = frequencyMultiplier[1] = 1000;
  duty[0] = duty[1] = 5000;
  amplitude[0] = amplitude[1] = 329;
  mode[0] = mode[1] = MODE_PWM;

  DrawChannel(0);
  DrawChannel(1);

  LcdUpdate();
}

void Process_Timer_Event(void)
{
  switch (buttonEvent)
  {
  case IDBUTTONB:
    if (currentChannel != 0)
    {
      cursorEnabled = CURSOR_TIMEOUT;
      currentChannel = 0;
      DrawSETS(0);
      DrawSETS(1);
    }
    else
    {
      if (!cursorEnabled)
      {
        cursorEnabled = CURSOR_TIMEOUT;
        DrawSETS(currentChannel);
      }
      else
      {
        cursorEnabled = CURSOR_TIMEOUT;
        ToggleOutput();
      }
    }
    break;
  case IDBUTTONA:
    if (currentChannel != 1)
    {
      cursorEnabled = CURSOR_TIMEOUT;
      currentChannel = 1;
      DrawSETS(0);
      DrawSETS(1);
    }
    else
    {
      if (!cursorEnabled)
      {
        cursorEnabled = CURSOR_TIMEOUT;
        DrawSETS(currentChannel);
      }
      else
      {
        cursorEnabled = CURSOR_TIMEOUT;
        ToggleOutput();
      }
    }
    break;
  case IDBUTTONCENTER:
    cursorEnabled = CURSOR_TIMEOUT;
    DrawSETS(currentChannel);
    break;
  case IDBUTTONTOP:
    if (cursorEnabled)
    {
      cursorEnabled = CURSOR_TIMEOUT;
      updateSETValues(1);
      DrawSETS(currentChannel);
    }
    break;
  case IDBUTTONBOTTOM:
    if (cursorEnabled)
    {
      cursorEnabled = CURSOR_TIMEOUT;
      updateSETValues(0);
      DrawSETS(currentChannel);
    }
    break;
  case IDBUTTONLEFT:
    if (cursorEnabled)
    {
      cursorEnabled = CURSOR_TIMEOUT;
      if (cursorPosition)
        cursorPosition--;
      else
        cursorPosition = CURSOR_POSITION_MAX;
      DrawSETS(currentChannel);
    }
    break;
  case IDBUTTONRIGHT:
    if (cursorEnabled)
    {
      cursorEnabled = CURSOR_TIMEOUT;
      if (cursorPosition < CURSOR_POSITION_MAX)
        cursorPosition++;
      else
        cursorPosition = 0;
      DrawSETS(currentChannel);
    }
    break;
  }

  if (cursorEnabled)
  {
    cursorEnabled--;
    if (!cursorEnabled)
      DrawSETS(currentChannel);
  }

  LcdUpdate();

  buttonEvent = IDNOEVENT;
}
