#ifndef chacha_h
#define chacha_h

#include <stdint.h>

typedef struct {
    uint32_t state[16];
    uint32_t working_state[16];
    unsigned int rounds;
    unsigned int word_index;
} ChaCha;

void chacha8_init(ChaCha *rng, const uint32_t seed[8], uint64_t stream);

void chacha8_zero(ChaCha *rng, uint64_t stream);

void chacha12_init(ChaCha *rng, const uint32_t seed[8], uint64_t stream);

void chacha12_zero(ChaCha *rng, uint64_t stream);

void chacha20_init(ChaCha *rng, const uint32_t seed[8], uint64_t stream);

void chacha20_zero(ChaCha *rng, uint64_t stream);

uint8_t chacha_u8(ChaCha *rng);

uint16_t chacha_u16(ChaCha *rng);

uint32_t chacha_u32(ChaCha *rng);

uint64_t chacha_u64(ChaCha *rng);

float chacha_f32(ChaCha *rng);

double chacha_f64(ChaCha *rng);

void chacha_fill_u8(ChaCha *rng, uint8_t *array, unsigned int count);

void chacha_fill_u16(ChaCha *rng, uint16_t *array, unsigned int count);

void chacha_fill_u32(ChaCha *rng, uint32_t *array, unsigned int count);

void chacha_fill_u64(ChaCha *rng, uint64_t *array, unsigned int count);

void chacha_fill_f32(ChaCha *rng, float *array, unsigned int count);

void chacha_fill_f64(ChaCha *rng, double *array, unsigned int count);

#endif /* chacha_h */
