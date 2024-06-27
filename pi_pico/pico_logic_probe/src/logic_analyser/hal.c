#include "logic_analyser.h"
#include "board.h"

void LAInit(void)
{
  LAInputsCommand(gpio_pull_up);
}

void LAInputsCommand(void (*func)(unsigned int))
{
  func(DATA0);
  func(DATA1);
  func(DATA2);
  func(DATA3);
  func(DATA4);
  func(DATA5);
  func(DATA6);
  func(DATA7);
  func(DATA8);
  func(DATA9);
  func(DATA10);
  func(DATA11);
  func(DATA12);
}
