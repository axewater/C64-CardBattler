#include "random.h"
#include <time.h>

/* Simple LCG (Linear Congruential Generator) */
static uint16_t rng_state = 0;

/* Initialize RNG with hardware timer seed */
void random_init(void) {
    /* Use CIA timer as seed - read from hardware */
    /* C64 CIA1 timer low byte at $DC04 */
    rng_state = *((uint8_t*)0xDC04);
    rng_state |= (*((uint8_t*)0xDC05) << 8);

    /* If seed is 0, use fallback */
    if (rng_state == 0) {
        rng_state = 12345;
    }
}

/* Generate random 8-bit number */
uint8_t random_get(void) {
    /* LCG: X(n+1) = (a * X(n) + c) mod m */
    /* Using parameters: a=75, c=74, m=65537 (16-bit) */
    rng_state = rng_state * 75 + 74;
    return (uint8_t)(rng_state >> 8);
}

/* Generate random number in range [min, max) */
uint8_t random_range(uint8_t min, uint8_t max) {
    if (min >= max) {
        return min;
    }
    return min + (random_get() % (max - min));
}

/* Fisher-Yates shuffle */
void random_shuffle(uint8_t* array, uint8_t size) {
    uint8_t i, j, temp;

    for (i = size - 1; i > 0; i--) {
        j = random_range(0, i + 1);

        /* Swap array[i] and array[j] */
        temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}
