#include "kinematics.h"

#include <stdbool.h>
#include <gb/metasprites.h>
#include <gb/gb.h>


static uint8_t fixed_point_to_pixel(uint16_t value)
{
    return value >> FIXED_POINT_POSITION_LENGTH;
}

void move_sprite_to_position(uint8_t sprite_num, Position position)
{
    move_sprite(
        sprite_num,
        fixed_point_to_pixel(position.x) + SPRITE_X_OFFSET,
        fixed_point_to_pixel(position.y) + SPRITE_Y_OFFSET
    );
}

void move_metasprite_to_position(Metasprite metasprite, Position position)
{
    uint8_t sprites_used = move_metasprite_ex(
        metasprite.metasprite,
        metasprite.start_tile_index,
        metasprite.base_props,
        metasprite.sprite_num,
        fixed_point_to_pixel(position.x) + SPRITE_X_OFFSET,
        fixed_point_to_pixel(position.y) + SPRITE_Y_OFFSET
    );

    // Hide any OAM slots reserved for this metasprite that this frame didn't
    // use (e.g. an animation frame with fewer sprites than a previous one).
    // Scoped to this metasprite's own reserved range (sprite_num..sprite_num
    // + max_sprite_count) so it can never reach into another entity's slots.
    hide_sprites_range(
        metasprite.sprite_num + sprites_used,
        metasprite.sprite_num + metasprite.max_sprite_count
    );
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