#ifndef KINEMATICS_H
#define KINEMATICS_H

// Position/Velocity use fixed-point values (real pixels * 16) instead of
// floats, since this CPU has no FPU. This lets velocity express sub-pixel
// speeds (e.g. 1.5 px/frame) via plain integer addition, with no drift —
// only converted down to whole pixels at render time.

#include <stdint.h>

#include "joypad.h"
#include "sprites.h"

#define FIXED_POINT_POSITION_LENGTH 4
#define VELOCITY_PER_FRAME 12

typedef struct Position {
    uint16_t x; // fixed-point
    uint16_t y; // fixed-point
} Position;

typedef enum Direction {
    DIRECTION_DOWN,
    DIRECTION_UP,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
} Direction;

typedef struct Velocity {
    int16_t x; // fixed-point; positive = right, negative = left
    int16_t y; // fixed-point; positive = down, negative = up
} Velocity;

// For entities that are a single hardware sprite (one 8x8/8x16 tile) — e.g.
// a simple projectile. This should not be used for sprites of more than a 
// single tile. For multiple-tile sprites use a metasprite.
void move_sprite_to_position(uint8_t sprite_num, Position position);

// For entities built from multiple hardware sprites arranged as one image
// (e.g. Player, a 16x16 sprite made of four 8x8 tiles). Positions the whole
// metasprite at once; per-tile offsets come from the Metasprite's own
// metasprite_t frame data, not from this call. Also hides any OAM slots in
// the metasprite's reserved range (see Metasprite.max_sprite_count) that
// this frame didn't use, so a smaller animation frame doesn't leave a
// previous frame's sprites stuck on screen.
void move_metasprite_to_position(Metasprite metasprite, Position position);
Velocity compute_velocity_from_joypad(JoypadState state);

#endif
