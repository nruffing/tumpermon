#ifndef RAND_UTIL_H
#define RAND_UTIL_H

#include <stdint.h>

void initialize_random(uint16_t seed);

// Inclusive random value in [min, max], built on GBDK's randw() (rand.h).
// Callers call initialize_random once before using this —
// unseeded, GBDK's generator produces the same sequence every run.
uint16_t rand_range(uint16_t min, uint16_t max);

#endif
