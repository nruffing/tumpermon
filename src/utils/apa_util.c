#include "apa_util.h"

#include <gb/drawing.h>
#include <gb/gb.h>
#include <string.h>

// APA is All Points Addressable graphics mode defined in `gb/drawing`

// GRAPHICS_WIDTH = 160 pixels, and since gotogxy uses 8-pixel character cells, that's 20 characters
// wide (160 / 8)
#define TOTAL_CHAR_WIDTH 20

void do_apa_mode(void)
{
    mode(M_DRAWING); // enter APA (All Points Addressable) graphics mode
}

void end_apa_mode(void)
{
    /**
     * This switches out of M_DRAWING, which uninstalls the APA-specific drawing_lcd interrupt
     * handler and returns the background tile layer to normal tile-map behavior — i.e. back to
     * regular printf()/tile-based rendering instead of per-pixel framebuffer drawing.
     *
     * The rest of the game doesn't pay the APA ISR overhead or accidentally have background tiles
     * treated as bitmap data.
     */
    mode(M_TEXT_OUT);
}

uint8_t get_centered_x(const char *str)
{
    uint8_t len = strlen(str);

    if (len >= TOTAL_CHAR_WIDTH) {
        return 0;
    }

    return (TOTAL_CHAR_WIDTH - len) / 2;
}

void print_centered(const char *str, uint8_t y)
{
    gotogxy(get_centered_x(str), y);
    gprint((char *)str);
}
