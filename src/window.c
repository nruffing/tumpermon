#include "window.h"

#include <gb/gb.h>

#include "font.h"

// Game Boy screen is 160x144px = 20x18 tiles. The window's tilemap is this
// size regardless of how much of it is actually visible/drawn to.
#define WINDOW_TILE_COLUMNS 20
#define WINDOW_TILE_ROWS 18

void clear_win(void)
{
    uint8_t blank_tile = char_to_font_tile(' ');
    for (uint8_t y = 0; y < WINDOW_TILE_ROWS; y++) {
        for (uint8_t x = 0; x < WINDOW_TILE_COLUMNS; x++) {
            set_win_tile_xy(x, y, blank_tile);
        }
    }
}

void draw_win_text(uint8_t x, uint8_t y, const char *text)
{
    while (*text) {
        set_win_tile_xy(x, y, char_to_font_tile(*text));
        x++;
        text++;
    }
}
