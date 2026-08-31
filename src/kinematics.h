#ifndef KINEMATICS_H
#define KINEMATICS_H

// Position/Velocity use fixed-point values (real pixels * 16) instead of
// floats, since this CPU has no FPU. This lets velocity express sub-pixel
// speeds (e.g. 1.5 px/frame) via plain integer addition, with no drift —
// only converted down to whole pixels at render time.

#include <gb/drawing.h>
#include <stdbool.h>
#include <stdint.h>

#include "joypad.h"
#include "sprites.h"

#define FIXED_POINT_POSITION_LENGTH 4
#define VELOCITY_PER_FRAME 12

// Position bounds of the visible screen, in fixed-point units (see above) —
// e.g. clamping/random-spawn callers like rand_range(MIN_POSITION_X,
// MAX_POSITION_X). These are screen bounds, not sprite bounds: they don't
// account for a given entity's tile size, so a sprite placed at
// MAX_POSITION_X/Y will have part of itself hang off the right/bottom edge
// (same as GRAPHICS_WIDTH/HEIGHT - 1 in pixel space) — callers that need a
// fully on-screen sprite should subtract that entity's width/height first.
#define MIN_POSITION_X 0
#define MIN_POSITION_Y 0
#define MAX_POSITION_X ((GRAPHICS_WIDTH - 1) << FIXED_POINT_POSITION_LENGTH)
#define MAX_POSITION_Y ((GRAPHICS_HEIGHT - 1) << FIXED_POINT_POSITION_LENGTH)

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

typedef struct PositionDelta {
    int16_t dx; // fixed-point; positive = right, negative = left
    int16_t dy; // fixed-point; positive = down, negative = up
} PositionDelta;

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

uint16_t absolute_distance(Position a, Position b);
PositionDelta position_delta(Position a, Position b); // delta relative to `a`
Velocity zero_velocity(void);

// Velocity of magnitude `speed` per axis, signed to move toward closing
// `delta` (i.e. applying the result via apply_*_velocity shrinks `delta`
// toward zero) — e.g. `velocity_toward_delta(position_delta(target,
// mover), speed)` chases `target` from `mover`. Clamped per-axis so a delta
// already smaller than `speed` on that axis returns exactly that delta
// rather than overshooting past zero and oscillating next frame.
Velocity velocity_toward_delta(PositionDelta delta, uint16_t speed);

#endif
