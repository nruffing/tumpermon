#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

// Fills the entire window tilemap with the font's blank tile. Unlike cls()
// (which only clears the *background* tilemap), the window tilemap is
// otherwise never touched by GBDK/hardware and starts out full of whatever
// garbage was left in that VRAM region — since background and window tiles
// share the same tile pattern pool (see sprites.h), those leftover indices
// can render as actual background art. Call before draw_win_text whenever
// the window is (re)opened, so only the cells you explicitly draw show
// anything.
void clear_win(void);

// Writes `text` as a horizontal row of window-layer tiles starting at tile
// coordinate (x, y). Unmapped characters (including space) draw as the
// blank tile — see font.h's char_to_font_tile.
void draw_win_text(uint8_t x, uint8_t y, const char *text);

#endif
