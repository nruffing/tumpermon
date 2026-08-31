#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

// WX (the window's X register, set via move_win's x arg) is offset from
// screen space by hardware design: WX=7 puts the window's left edge at
// screen pixel 0. WX 0-6 shifts the window partially off-screen (and WX=0
// is glitchy on real DMG hardware), so this offset can't be zero.
#define WX_SCREEN_ORIGIN_OFFSET 7

void initialize_win(void);

void move_win_fullscreen(void);

// Fills the entire window tilemap with the font's blank tile. Unlike cls()
// (which only clears the *background* tilemap), the window tilemap is
// otherwise never touched by GBDK/hardware and starts out full of whatever
// garbage was left in that VRAM region — since background and window tiles
// share the same tile pattern pool (see sprites.h), those leftover indices
// can render as actual background art. Call before draw_win_text whenever
// the window is (re)opened, so only the cells you explicitly draw show
// anything.
void clear_win(void);

uint8_t get_win_position_x(uint8_t pixel_x);

// Writes `text` as a horizontal row of window-layer tiles starting at tile
// coordinate (x, y). Unmapped characters (including space) draw as the
// blank tile — see font.h's char_to_font_tile.
void draw_win_text(uint8_t x, uint8_t y, const char *text);

// Positions the window as a row `width` tiles wide (clamped to the window's
// 20-tile-column width), flush against the screen's top-right corner. This
// only sets the window's origin — hardware always extends the window from
// there to the screen's bottom-right corner, so rows below the top one
// still show through whatever's drawn (or left over) in them unless kept
// blank (see clear_win) or the window is later repositioned/hidden.
void move_win_single_row_top_right(uint8_t width, uint8_t padding_px);

#endif
