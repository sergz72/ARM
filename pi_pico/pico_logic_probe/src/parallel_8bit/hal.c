#include "board.h"
#include "parallel_8bit.h"
#include "../logic_analyser/logic_analyser.h"

void P8Init(void)
{
  LAInputsCommand(gpio_pull_up);
}
