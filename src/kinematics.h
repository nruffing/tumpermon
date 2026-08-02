#ifndef KINEMATICS_H
#define KINEMATICS_H

// Position/Velocity use fixed-point values (real pixels * 16) instead of
// floats, since this CPU has no FPU. This lets velocity express sub-pixel
// speeds (e.g. 1.5 px/frame) via plain integer addition, with no drift —
// only converted down to whole pixels at render time.

#include <stdint.h>

#include "joypad.h"

#define FIXED_POINT_POSITION_LENGTH 4

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

void move_sprite_to_position(uint8_t sprite_num, Position position);
Velocity compute_velocity_from_joypad(JoypadState state);

#endif
