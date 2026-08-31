#include "kinematics.h"

#include <gb/gb.h>
#include <gb/metasprites.h>
#include <stdbool.h>

#include "utils/tick_util.h"

uint8_t fixed_point_to_pixel(uint16_t value)
{
    return value >> FIXED_POINT_POSITION_LENGTH;
}

uint16_t pixel_to_fixed_point(uint8_t pixel)
{
    return (uint16_t)pixel << FIXED_POINT_POSITION_LENGTH;
}

void move_sprite_to_position(uint8_t sprite_num, Position position)
{
    move_sprite(
        sprite_num,
        fixed_point_to_pixel(position.x) + SPRITE_X_OFFSET,
        fixed_point_to_pixel(position.y) + SPRITE_Y_OFFSET);
}

static void move_metasprite_ref(MetaspriteRef ref, MetaspriteMetadata metadata, Position position)
{
    uint8_t new_x = fixed_point_to_pixel(position.x) + SPRITE_X_OFFSET;
    uint8_t new_y = fixed_point_to_pixel(position.y) + SPRITE_Y_OFFSET;
    uint8_t sprites_used = 0;

    if (ref.flip_x) {
        sprites_used = move_metasprite_flipx(
            ref.metasprite,
            metadata.start_tile_index,
            metadata.base_props,
            metadata.sprite_num,
            new_x,
            new_y);
    } else {
        sprites_used = move_metasprite_ex(
            ref.metasprite,
            metadata.start_tile_index,
            metadata.base_props,
            metadata.sprite_num,
            new_x,
            new_y);
    }

    // Hide any OAM slots reserved for this metasprite that this frame didn't
    // use (e.g. an animation frame with fewer sprites than a previous one).
    // Scoped to this metasprite's own reserved range (sprite_num..sprite_num
    // + max_sprite_count) so it can never reach into another entity's slots.
    if (sprites_used > 0) {
        hide_sprites_range(
            metadata.sprite_num + sprites_used,
            metadata.sprite_num + metadata.max_sprite_count);
    }
}

void move_metasprite_to_position(Metasprite metasprite, Position position)
{
    move_metasprite_ref(metasprite.ref, metasprite.metadata, position);
}

void move_animated_metasprite_to_position(
    AnimatedMetasprite metasprite,
    Position position,
    Direction direction)
{
    MetaspriteRef ref = metasprite.frames[direction].refs[0];
    move_metasprite_ref(ref, metasprite.metadata, position);
}

Velocity compute_velocity_from_joypad(
    const JoypadState *state,
    const KinematicBehaviorContext *kinematics)
{
    int16_t vel_x = 0, vel_y = 0;
    uint16_t y_pressed_at_tick = 0, x_pressed_at_tick = 0;

    // Opposing directions held simultaneously cancel out to 0, rather than
    // one arbitrarily winning.
    if (state->up.is_pressed && !state->down.is_pressed) {
        vel_y = -1 * VELOCITY_PER_FRAME;
        y_pressed_at_tick = state->up.pressed_at_tick;
    } else if (state->down.is_pressed && !state->up.is_pressed) {
        vel_y = VELOCITY_PER_FRAME;
        y_pressed_at_tick = state->down.pressed_at_tick;
    }

    if (state->left.is_pressed && !state->right.is_pressed) {
        vel_x = -1 * VELOCITY_PER_FRAME;
        x_pressed_at_tick = state->left.pressed_at_tick;
    } else if (state->right.is_pressed && !state->left.is_pressed) {
        vel_x = VELOCITY_PER_FRAME;
        x_pressed_at_tick = state->right.pressed_at_tick;
    }

    // When diagonal movement isn't allowed and both axes are active,
    // whichever direction was pressed more recently wins and the other
    // axis is dropped. Ties (tick_diff == 0 — e.g. both held since before
    // either changed) keep vertical, matching apply_player_velocity's
    // facing-priority tie-break in player.c.
    bool both_axes_active = (vel_x != 0) && (vel_y != 0);
    if (!kinematics->allow_diagonal_movement && both_axes_active) {
        int16_t tick_diff = compare_ticks(x_pressed_at_tick, y_pressed_at_tick);
        if (tick_diff > 0) {
            vel_y = 0;
        } else {
            vel_x = 0;
        }
    }

    Velocity velocity = { .x = vel_x, .y = vel_y };
    return velocity;
}

static uint16_t abs_diff(uint16_t a, uint16_t b)
{
    return a > b ? a - b : b - a;
}

uint16_t absolute_distance(Position a, Position b)
{
    uint16_t dx = abs_diff(a.x, b.x);
    uint16_t dy = abs_diff(a.y, b.y);
    return dx + dy;
}

bool aabb_overlaps(
    Position a_position,
    uint8_t a_width,
    uint8_t a_height,
    Position b_position,
    uint8_t b_width,
    uint8_t b_height,
    uint8_t buffer_px)
{
    uint16_t a_width_fp = pixel_to_fixed_point(a_width);
    uint16_t a_height_fp = pixel_to_fixed_point(a_height);
    uint16_t b_width_fp = pixel_to_fixed_point(b_width);
    uint16_t b_height_fp = pixel_to_fixed_point(b_height);
    uint16_t buffer_fp = pixel_to_fixed_point(buffer_px);

    // Inset both boxes by buffer_fp on every side, applied directly to the
    // edge comparisons rather than shrinking width/height first, to avoid
    // underflow if a box is narrower/shorter than 2 * buffer_px.
    return a_position.x + buffer_fp < b_position.x + b_width_fp - buffer_fp &&
           a_position.x + a_width_fp - buffer_fp > b_position.x + buffer_fp &&
           a_position.y + buffer_fp < b_position.y + b_height_fp - buffer_fp &&
           a_position.y + a_height_fp - buffer_fp > b_position.y + buffer_fp;
}

PositionDelta position_delta(Position a, Position b)
{
    PositionDelta delta = { .dx = a.x - b.x, .dy = a.y - b.y };
    return delta;
}

Velocity zero_velocity(void)
{
    Velocity zero = { .x = 0, .y = 0 };
    return zero;
}

static int16_t axis_velocity_toward(int16_t delta, int16_t speed)
{
    if (delta > 0) {
        return delta < speed ? delta : speed;
    } else if (delta < 0) {
        return delta > -speed ? delta : -speed;
    }
    return 0;
}

Velocity velocity_toward_delta(PositionDelta delta, uint16_t speed)
{
    int16_t signed_speed = (int16_t)speed;
    Velocity velocity = { .x = axis_velocity_toward(delta.dx, signed_speed),
                          .y = axis_velocity_toward(delta.dy, signed_speed) };
    return velocity;
}