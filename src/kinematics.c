#include "kinematics.h"

#include <stdbool.h>
#include <gb/gb.h>

// move_sprite() coordinates are offset — the hardware reserves an 8px/16px
// margin for sprites scrolling on/off the top and left edges, so x=8, y=16
// is actually the sprite's top-left visible at screen (0,0).
#define SPRITE_X_OFFSET 8
#define SPRITE_Y_OFFSET 16

#define VELOCITY_PER_FRAME 12

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