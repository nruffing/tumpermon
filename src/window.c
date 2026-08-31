#include "window.h"

#include <gb/gb.h>

#include "font.h"

// Game Boy screen is 160x144px = 20x18 tiles. The window's tilemap is this
// size regardless of how much of it is actually visible/drawn to.
#define WINDOW_TILE_COLUMNS 20
#define WINDOW_TILE_ROWS 18

#define WINDOW_TILE_WIDTH_PX 8

void initialize_win(void)
{
    HIDE_WIN;
    move_win_fullscreen();
    clear_win();
    SHOW_WIN;
}

void move_win_fullscreen(void)
{
    move_win(get_win_position_x(0), 0);
}

void clear_win(void)
{
    uint8_t blank_tile = char_to_font_tile(' ');
    for (uint8_t y = 0; y < WINDOW_TILE_ROWS; y++) {
        for (uint8_t x = 0; x < WINDOW_TILE_COLUMNS; x++) {
            set_win_tile_xy(x, y, blank_tile);
        }
    }
}

uint8_t get_win_position_x(uint8_t pixel_x)
{
    return pixel_x + WX_SCREEN_ORIGIN_OFFSET;
}

void draw_win_text(uint8_t x, uint8_t y, const char *text)
{
    while (*text) {
        set_win_tile_xy(x, y, char_to_font_tile(*text));
        x++;
        text++;
    }
}

void move_win_single_row_top_right(uint8_t width, uint8_t padding_px)
{
    uint8_t clamped_width = width > WINDOW_TILE_COLUMNS ? WINDOW_TILE_COLUMNS : width;
    uint8_t tile_x = WINDOW_TILE_COLUMNS - clamped_width;
    move_win(get_win_position_x(tile_x * WINDOW_TILE_WIDTH_PX - padding_px), padding_px);
}
