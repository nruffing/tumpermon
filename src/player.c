#include "player.h"

#include <gb/gb.h>

#include "kinematics.h"


const uint8_t player_sprite_tile[16] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

void initialize_player_sprite_tiles(uint16_t start_index)
{
    // load the tile data into VRAM's sprite tile table once
    set_sprite_data(start_index, 1, player_sprite_tile);
}

Player initialize_player(uint8_t sprite_num, Position position)
{
    Player player = { .position = position };
    move_sprite_to_position(sprite_num, position);
    return player;
}