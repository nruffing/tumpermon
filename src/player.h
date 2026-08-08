#ifndef PLAYER_H
#define PLAYER_H

#include "kinematics.h"
#include "sprites.h"

typedef struct Player {
    Metasprite metasprite;
    Position position;
    Direction direction;
} Player;

Player initialize_player(Position position, Metasprite metasprite);
void apply_velocity(Player *player, Velocity velocity);
void update_player_sprite(Player *player);

#endif
