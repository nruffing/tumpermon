#include "player.h"

#include <gb/gb.h>

#include "kinematics.h"
#include "sprites.h"


const uint8_t player_sprite_tile[16] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

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