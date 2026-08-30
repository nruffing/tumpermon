#ifndef PLAYER_H
#define PLAYER_H

#include "kinematics.h"
#include "sprites.h"

typedef struct Player {
    AnimatedMetasprite metasprite;
    Position position;
    Direction direction;
} Player;

AnimatedMetasprite create_player_metasprite(void);
Player initialize_player(Position position, Direction direction, AnimatedMetasprite metasprite);
void apply_player_velocity(Player *player, Velocity velocity);
void update_player_sprite(Player *player);

#endif
