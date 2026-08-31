#ifndef PLAYER_H
#define PLAYER_H

#include "kinematics.h"
#include "sprites.h"

#define INITIAL_PLAYER_HIT_POINT 100

typedef struct PlayerPreviousFrame {
    uint8_t hit_points;
} PlayerPreviousFrame;

typedef struct Player {
    AnimatedMetasprite metasprite;
    Position position;
    Direction direction;
    uint8_t hit_points;
    PlayerPreviousFrame previous_frame;
} Player;

AnimatedMetasprite create_player_metasprite(void);
Player initialize_player(Position position, Direction direction, AnimatedMetasprite metasprite);
void apply_player_velocity(Player *player, Velocity velocity);
void update_player_sprite(Player *player);
void update_player_previous_frame_state(Player *player);

#endif
