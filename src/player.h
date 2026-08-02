#ifndef PLAYER_H
#define PLAYER_H

#include "kinematics.h"

typedef struct Player {
    Position position;
    Direction direction;
} Player;

void initialize_player_sprite_tiles(uint16_t start_index);
Player initialize_player(Position position);
void apply_velocity(Player *player, Velocity velocity);
void update_player_sprite(Player *player);

#endif
