#include "player.h"

#include <gb/gb.h>

#include "kinematics.h"
#include "sprites.h"


const uint8_t player_sprite_tile[16] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

// unsigned char TilesetTiles[] =
// {
//     0x07,0x07,0x08,0x08,0x10,0x10,0x10,0x10,0x20,0x20,0x20,0x20,0x52,0x52,0x42,0x42,
//     0xE0,0xE0,0x10,0x10,0x08,0x08,0x08,0x08,0x04,0x04,0x04,0x04,0x4A,0x4A,0x42,0x42,
//     0x20,0x20,0x38,0x38,0x47,0x47,0x40,0x40,0x20,0x20,0x10,0x10,0x11,0x11,0x0E,0x0E,
//     0x04,0x04,0x1C,0x1C,0xE2,0xE2,0x02,0x02,0x04,0x04,0x08,0x08,0x88,0x88,0x70,0x70,
// };

// unsigned char TilesetMap[] =
// {
//     0x00,0x01,
//     0x02,0x03,
// };

void initialize_player_sprite_tiles(uint16_t start_index)
{
    // load the tile data into VRAM's sprite tile table once
    set_sprite_data(start_index, 1, player_sprite_tile);
}

Player initialize_player(Position position)
{
    Player player = { .position = position, .direction = DIRECTION_DOWN };
    update_player_sprite(&player);
    return player;
}

void apply_velocity(Player *player, Velocity velocity)
{
    player->position.x += velocity.x;
    player->position.y += velocity.y;

    // Prefer facing vertically over horizontally when moving diagonally.
    // Standing still (both zero) keeps whatever direction was last faced.
    if (velocity.y < 0) {
        player->direction = DIRECTION_UP;
    } else if (velocity.y > 0) {
        player->direction = DIRECTION_DOWN;
    } else if (velocity.x < 0) {
        player->direction = DIRECTION_LEFT;
    } else if (velocity.x > 0) {
        player->direction = DIRECTION_RIGHT;
    }
}

void update_player_sprite(Player *player)
{
    move_sprite_to_position(PLAYER_SPRITE_SLOT, player->position);
}