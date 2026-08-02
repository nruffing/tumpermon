#ifndef PLAYER_H
#define PLAYER_H

#include "position.h"

typedef struct Player {
    Position position;
} Player;

void initialize_player_sprite_tiles(uint16_t start_index);
Player initialize_player(uint8_t sprite_num, Position position);

#endif
