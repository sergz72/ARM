#ifndef STM32F334_KBD_TEST_MPR121_MPR121_H
#define STM32F334_KBD_TEST_MPR121_MPR121_H

typedef struct {
  unsigned char touch_threshold, release_threshold;
} MPR121Thresholds;

int MPR121Init(const MPR121Thresholds *thresholds);
int MPR121GetTouch(int *flags);
unsigned char MPR121KbdHandler(void);

int MPR121WriteRegister(unsigned char reg, unsigned char value);
int MPR121ReadRegister(unsigned char reg, unsigned char *value);

#endif //STM32F334_KBD_TEST_MPR121_MPR121_H
