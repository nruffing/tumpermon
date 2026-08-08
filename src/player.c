#include "player.h"

#include <gb/gb.h>

#include <res/player.h>

#include "kinematics.h"


Player initialize_player(Position position, Metasprite metasprite)
{
    Player player = { .metasprite = metasprite, .position = position, .direction = DIRECTION_DOWN };
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
    move_metasprite_to_position(player->metasprite, player->position);
}