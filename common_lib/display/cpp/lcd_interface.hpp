#ifndef _LCD_INTERFACE_HPP
#define _LCD_INTERFACE_HPP

template<typename T>
class LCDInterface
{
public:
  virtual void SendCommand(T command, const T *data, unsigned int count) = 0;
};

template<typename T>
class LCDParallelGPIOInterface: public LCDInterface<T>
{
private:
  void DataWrite(T data)
  {
    DataSet(data);
    WRClr();
    WRDelay();
    WRSet();
    WRDelay();
  }
protected:
  virtual void CSClr() = 0;
  virtual void CSSet() = 0;
  virtual void DCClr() = 0;
  virtual void DCSet() = 0;
  virtual void WRClr() = 0;
  virtual void WRSet() = 0;
  virtual void DataSet(T data) = 0;
  virtual void WRDelay() = 0;
public:
  void SendCommand(T command, const T *data, unsigned int count)
  {
    DCClr();
    CSClr();
    DataWrite(command);
    if (count)
    {
      DCSet();
      while (count--)
        DataWrite(*data++);
    }
    CSSet();
  }

  void Test()
  {
    CSClr();
    CSSet();
    DCSet();
    DCClr();
    WRClr();
    WRSet();
    DataSet(0);
    T value = 1;
    int counter = sizeof(T) * 8;
    while (counter--)
    {
      DataSet(value);
      value <<= 1;
    }
  }
};

template<typename T>
class LCDParallelFMCInterface: public LCDInterface<T>
{
private:
  T *commandAddress;
  T *dataAddress;
public:
  LCDParallelFMCInterface(T *_commandAddress, T *_dataAddress)
  {
    commandAddress = _commandAddress;
    dataAddress = _dataAddress;
  }

  void SendCommand(T command, const T *data, unsigned int count) override
  {
    *commandAddress = command;
    while (count--)
      *dataAddress = *data++;
  }
};

#endif