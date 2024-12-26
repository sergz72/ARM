#ifndef _LCD_ILI9341_HPP
#define _LCD_ILI9341_HPP

#include "lcd.hpp"
#include "lcd_interface.hpp"

class LCD_ILI9341: public LCD<unsigned short>
{
private:
  LCDInterface<unsigned char> *interface;

  void SleepOut();
protected:
  virtual void ResetSet() = 0;
  unsigned int GetBufferOffset(unsigned int x, unsigned int y) override;
  unsigned int InternalUpdate(bool all) override;
  void SetHWOrientation() override;
public:
  inline LCD_ILI9341(int _orientation, unsigned short *_buffer1, unsigned short *_buffer2, LCDInterface<unsigned char> *_interface):
    LCD<unsigned short>(_orientation, 320, 240, _buffer1, _buffer2)
  {
    interface = _interface;
  }

  void On() override;
  void Off() override;

  void Init() override;

  static inline unsigned short RGB(unsigned short R, unsigned short G, unsigned short B)
  {
    return 0;
  }

  static const unsigned short WHITE_COLOR = 0xFFFF;
};

#endif