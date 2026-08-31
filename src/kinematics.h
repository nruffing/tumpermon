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

// Fixed-point <-> raw-pixel conversion — see the note above on why
// Position/Velocity are fixed-point at all. fixed_point_to_pixel truncates
// (no rounding) since that's what rendering needs (whole tile/pixel
// coordinates); pixel_to_fixed_point is its exact inverse (no precision
// loss going the other way — every raw pixel maps to a whole number of
// fixed-point units).
uint8_t fixed_point_to_pixel(uint16_t value);
uint16_t pixel_to_fixed_point(uint8_t pixel);

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

// Axis-aligned bounding box overlap test between two entities' rectangles —
// each given as its top-left Position plus width/height. Position is
// fixed-point (see above) but widths/heights are raw pixels — converted to
// fixed-point internally via pixel_to_fixed_point, so callers pass an
// entity's plain size constant (e.g. PLAYER_SPRITE_WIDTH_PX) directly.
// buffer_px insets both boxes by that many pixels on every side before
// testing, so a shallow graze along an edge doesn't count as a hit — the
// boxes need to overlap by more than 2 * buffer_px combined before this
// returns true. Pass 0 for an exact/unforgiving hitbox.
bool aabb_overlaps(
    Position a_position,
    uint8_t a_width,
    uint8_t a_height,
    Position b_position,
    uint8_t b_width,
    uint8_t b_height,
    uint8_t buffer_px);
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
