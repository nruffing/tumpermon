#ifndef SPRITES_H
#define SPRITES_H

#include <gb/metasprites.h>
#include <stdbool.h>
#include <stdint.h>

// move_sprite() coordinates are offset — the hardware reserves an 8px/16px
// margin for sprites scrolling on/off the top and left edges, so x=8, y=16
// is actually the sprite's top-left visible at screen (0,0).
#define SPRITE_X_OFFSET 8
#define SPRITE_Y_OFFSET 16

#define PLAYER_SPRITE_TILE_START_INDEX 0
#define PLAYER_SPRITE_SLOT 0
// Max hardware OAM sprites any single player metasprite frame can ever use
// (each frame is still a 16x16 = 2x2-tile image = 4 sprites, regardless of
// how many animation frames exist). Reserves
// PLAYER_SPRITE_SLOT..PLAYER_SPRITE_SLOT+PLAYER_SPRITE_MAX_COUNT-1 for the
// player exclusively — move_metasprite_to_position uses this to scope its
// hide_sprites_range cleanup so it never reaches into another entity's slots.
#define PLAYER_SPRITE_MAX_COUNT 4

#define METASPRITE_MAX_ANIMATION_FRAMES 3 // widen later if some entity needs more

// base sprite property flags (can be used to set palette, etc)
#define DEFAULT_METASPRITE_BASE_PROP 0

typedef struct MetaspriteMetadata {
    uint8_t start_tile_index;
    uint8_t sprite_num;
    uint8_t base_props;
    // Max hardware OAM sprites any frame of this metasprite can use — the
    // size of its reserved OAM slot range (see e.g. PLAYER_SPRITE_MAX_COUNT).
    uint8_t max_sprite_count;
} MetaspriteMetadata;

typedef struct MetaspriteRef {
    const metasprite_t *metasprite;
    bool flip_x;
} MetaspriteRef;

typedef struct Metasprite {
    MetaspriteRef ref;
    MetaspriteMetadata metadata;
} Metasprite;

typedef struct MetaspriteAnimationFrames {
    MetaspriteRef refs[METASPRITE_MAX_ANIMATION_FRAMES];
} MetaspriteAnimationFrames;

typedef struct AnimatedMetasprite {
    // indexed by Direction (kinematics.h)
    // example usage: MetaspriteRef ref =
    // player->metasprite.frames[player->direction].refs[player->animation_frame];
    MetaspriteAnimationFrames frames[4];
    MetaspriteMetadata metadata;
} AnimatedMetasprite;

#endif
