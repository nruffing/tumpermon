#include "kinematics.h"

#include <gb/gb.h>

// move_sprite() coordinates are offset — the hardware reserves an 8px/16px
// margin for sprites scrolling on/off the top and left edges, so x=8, y=16
// is actually the sprite's top-left visible at screen (0,0).
#define SPRITE_X_OFFSET 8
#define SPRITE_Y_OFFSET 16

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