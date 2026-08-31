#ifndef INT_UTIL_H
#define INT_UTIL_H

#include <stdint.h>

#define DECIMAL_RADIX 10
#define UINT8_TO_STR_BUFFER_SIZE 4

// Writes val as a fixed-width, space-padded, right-aligned 3-digit decimal
// field into out (must be at least UINT8_TO_STR_BUFFER_SIZE bytes) — e.g.
// 5 -> "  5", 42 -> " 42", 255 -> "255". Hand-rolled rather than going
// through uitoa+strlen+strcpy+pad-loop: uint8_t's range is fixed and small
// enough that the digit count and padding fall directly out of two
// divisions, so there's no need to measure a variable-length result
// afterward or pull in string.h at all.
void uint8_to_padded3(uint8_t val, char *out);

#endif