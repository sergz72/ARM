#ifndef _BOARD_H
#define _BOARD_H

#ifndef NULL
#define NULL 0
#endif

#define FALSE 0
#define TRUE  1

#define MAX_SHELL_COMMANDS 100
#define MAX_SHELL_COMMAND_PARAMETERS 10
#define MAX_SHELL_COMMAND_PARAMETER_LENGTH 50
#define SHELL_HISTORY_SIZE 20
#define SHELL_HISTORY_ITEM_LENGTH 100

#define HYPERRAM_DQ0      0
#define HYPERRAM_DQ1      1
#define HYPERRAM_DQ2      2
#define HYPERRAM_DQ3      3
#define HYPERRAM_DQ4      4
#define HYPERRAM_DQ5      5
#define HYPERRAM_DQ6      6
#define HYPERRAM_DQ7      7
#define HYPERRAM_RESET    8
#define HYPERRAM_RWDS     9
#define HYPERRAM_CS       10
#define HYPERRAM_CK       11
#define HYPERRAM_IO_DIR   12
#define HYPERRAM_RWDS_DIR 13

void SystemInit(void);

#endif
