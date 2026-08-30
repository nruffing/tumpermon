#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>

#include "kinematics.h"

typedef struct Enemy {
    uint8_t sprite_num;
    Position position;
} Enemy;

Enemy initialize_enemy(uint8_t enemy_index, Position position);
void apply_enemy_velocity(Enemy *enemy, Velocity velocity);
void update_enemy_sprite(Enemy *enemy);

#endif