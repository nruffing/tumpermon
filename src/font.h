#ifndef FONT_H
#define FONT_H

#include <stdint.h>

// Maps an ASCII character to its VRAM tile index within the loaded font
// tileset (res/font.png, FONT_TILE_START_INDEX — see sprites.h/main.c's
// initialize_font). font.png is laid out uppercase/lowercase-shared (both
// cases draw the same glyph); unmapped characters (including space) map to
// the blank tile.
uint8_t char_to_font_tile(char c);

#endif
