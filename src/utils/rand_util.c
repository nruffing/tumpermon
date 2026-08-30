#include <gb/gb.h>

#include "rand_util.h"

#include <rand.h>

void initialize_random(uint16_t seed)
{
    // DIV_REG increments at a fixed rate (16384 Hz, or every 256 CPU cycles) continuously, from the
    // moment the system powers on.
    uint8_t more_entropy = (uint8_t)(seed ^ DIV_REG);
    initrand(more_entropy);
}

uint16_t rand_range(uint16_t min, uint16_t max)
{
    return min + (randw() % (max - min + 1));
}
