#ifndef _TH02_FUNC_H
#define _TH02_FUNC_H

unsigned int TH02Start(void);
void TH02Loop(unsigned int rc);

extern int prev_temperature;
extern unsigned int prev_humi;

#endif
