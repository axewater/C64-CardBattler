#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

/* RNG Functions */
void random_init(void);
uint8_t random_get(void);
uint8_t random_range(uint8_t min, uint8_t max);
void random_shuffle(uint8_t* array, uint8_t size);

#endif /* RANDOM_H */
