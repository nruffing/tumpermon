#include "int_util.h"

void uint8_to_padded3(uint8_t val, char *out)
{
    uint8_t hundreds = val / 100;
    uint8_t remainder = val % 100;
    uint8_t tens = remainder / 10;
    uint8_t ones = remainder % 10;

    out[0] = hundreds ? (char)('0' + hundreds) : ' ';
    out[1] = (hundreds || tens) ? (char)('0' + tens) : ' ';
    out[2] = (char)('0' + ones);
    out[3] = '\0';
}
