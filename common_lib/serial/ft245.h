#ifndef FT245_H_
#define FT245_H_

void FT245Init(void (*read_handler)(char, int), void (*end_handler)(int));
void FT245Write(unsigned char);
char FT245Read(void);
void FT245_ISR(void);

#endif /*FT245_H_*/
