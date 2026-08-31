#ifndef PLAYER_H
#define PLAYER_H

#include "kinematics.h"
#include "sprites.h"

#define INITIAL_PLAYER_HIT_POINTS 100
#define INITIAL_PREVIOUS_FRAME_PLAYER_HIT_POINTS 0

#define PLAYER_SPRITE_HEIGHT_PX 16
#define PLAYER_SPRITE_WIDTH_PX 16

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
PlayerPreviousFrame create_initial_player_previous_frame(void);
Player initialize_player(Position position, Direction direction, AnimatedMetasprite metasprite);
void apply_player_velocity(Player *player, Velocity velocity);
void apply_player_damage(Player *player, uint8_t hit_points);
void update_player_sprite(Player *player);
void update_player_previous_frame_state(Player *player);
Position get_player_sprite_top_left_position(Player *player);

#endif
