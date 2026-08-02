#include "apa_util.h"

#include <gb/gb.h>
#include <gb/drawing.h>
#include <string.h>

// APA is All Points Addressable graphics mode defined in `gb/drawing`

// GRAPHICS_WIDTH = 160 pixels, and since gotogxy uses 8-pixel character cells, that's 20 characters wide (160 / 8)
#define TOTAL_CHAR_WIDTH 20


void do_apa_mode(void)
{
    mode(M_DRAWING); // enter APA (All Points Addressable) graphics mode
}

uint8_t get_centered_x(const char *str)
{
    uint8_t len = strlen(str);

    if (len >= TOTAL_CHAR_WIDTH) {
        return 0;
    }

    return (TOTAL_CHAR_WIDTH - len) / 2;
}
