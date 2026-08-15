#include "kinematics.h"

#include <gb/gb.h>
#include <gb/metasprites.h>
#include <stdbool.h>

static uint8_t fixed_point_to_pixel(uint16_t value)
{
    return value >> FIXED_POINT_POSITION_LENGTH;
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

Velocity compute_velocity_from_joypad(JoypadState state)
{
    int16_t vel_x = 0, vel_y = 0;

    // Opposing directions held simultaneously cancel out to 0, rather than
    // one arbitrarily winning.
    if (state.up.is_pressed && !state.down.is_pressed) {
        vel_y = -1 * VELOCITY_PER_FRAME;
    } else if (state.down.is_pressed && !state.up.is_pressed) {
        vel_y = VELOCITY_PER_FRAME;
    }

    if (state.left.is_pressed && !state.right.is_pressed) {
        vel_x = -1 * VELOCITY_PER_FRAME;
    } else if (state.right.is_pressed && !state.left.is_pressed) {
        vel_x = VELOCITY_PER_FRAME;
    }

    Velocity velocity = { .x = vel_x, .y = vel_y };
    return velocity;
}