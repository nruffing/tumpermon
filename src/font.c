#include "font.h"

#include "sprites.h"

// mirrors examples/cross-platform/text_basic's GetCharacterVRamTile, which
// is the layout res/font.png was copied from.
uint8_t char_to_font_tile(char c)
{
    uint8_t tile = 0;

    if (c >= 'a' && c <= 'z') {
        tile = (uint8_t)(c - 'a') + 1;
    } else if (c >= 'A' && c <= 'Z') {
        tile = (uint8_t)(c - 'A') + 1;
    } else if (c >= '0' && c <= '9') {
        tile = (uint8_t)(c - '0') + 27;
    } else {
        switch (c) {
        case '!':
            tile = 37;
            break;
        case ':':
            tile = 38;
            break;
        case '?':
            tile = 39;
            break;
        case '/':
            tile = 40;
            break;
        case '=':
            tile = 41;
            break;
        case ',':
            tile = 42;
            break;
        case '.':
            tile = 43;
            break;
        case '<':
            tile = 44;
            break;
        case '>':
            tile = 45;
            break;
        default:
            break; // includes ' ' — falls through to the blank tile (0)
        }
    }

    return (uint8_t)(tile + FONT_TILE_START_INDEX);
}
