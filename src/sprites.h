#ifndef SPRITES_H
#define SPRITES_H

#include <gb/metasprites.h>

// move_sprite() coordinates are offset — the hardware reserves an 8px/16px
// margin for sprites scrolling on/off the top and left edges, so x=8, y=16
// is actually the sprite's top-left visible at screen (0,0).
#define SPRITE_X_OFFSET 8
#define SPRITE_Y_OFFSET 16

#define PLAYER_SPRITE_TILE_START_INDEX 0 // length 4
#define PLAYER_SPRITE_SLOT 0
// Max hardware OAM sprites any player metasprite frame can ever use. Reserves
// PLAYER_SPRITE_SLOT..PLAYER_SPRITE_SLOT+PLAYER_SPRITE_MAX_COUNT-1 for the
// player exclusively — move_metasprite_to_position uses this to scope its
// hide_sprites_range cleanup so it never reaches into another entity's slots.
#define PLAYER_SPRITE_MAX_COUNT 4

// base sprite property flags (can be used to set palette, etc)
#define DEFAULT_METASPRITE_BASE_PROP 0
typedef struct Metasprite {
    const metasprite_t *metasprite;
    uint8_t start_tile_index;
    uint8_t sprite_num;
    uint8_t base_props;
    // Max hardware OAM sprites any frame of this metasprite can use — the
    // size of its reserved OAM slot range (see e.g. PLAYER_SPRITE_MAX_COUNT).
    uint8_t max_sprite_count;
} Metasprite;

#endif
