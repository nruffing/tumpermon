#ifndef KINEMATICS_H
#define KINEMATICS_H

// Position/Velocity use fixed-point values (real pixels * 16) instead of
// floats, since this CPU has no FPU. This lets velocity express sub-pixel
// speeds (e.g. 1.5 px/frame) via plain integer addition, with no drift —
// only converted down to whole pixels at render time.

#include <stdbool.h>
#include <stdint.h>

#include "joypad.h"
#include "sprites.h"

#define FIXED_POINT_POSITION_LENGTH 4
#define VELOCITY_PER_FRAME 12

typedef struct KinematicBehaviorContext {
    bool allow_diagonal_movement;
} KinematicBehaviorContext;

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
void move_metasprite_to_position(Metasprite metasprite, Position position);
void move_animated_metasprite_to_position(
    AnimatedMetasprite metasprite,
    Position position,
    Direction direction);

// Takes both by pointer rather than by value — JoypadState in particular is
// large enough (8 InputStates) that passing it by value on this platform's
// constrained stack/calling convention risks corruption; see the diagonal-
// movement bug this fixed (kinematics.allow_diagonal_movement was
// intermittently reading true when it should always be false).
Velocity compute_velocity_from_joypad(
    const JoypadState *state,
    const KinematicBehaviorContext *kinematics);

#endif
