#ifndef _TRNG_H
#define _TRNG_H

void trng_init(void);
void trng_generate(unsigned int *data, unsigned int length);

#endif
