#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>

#include "behavior/node.h"
#include "kinematics.h"

#define DEFAULT_ENEMY_VELOCITY_X 0
#define DEFAULT_ENEMY_VELOCITY_Y 0

typedef struct Enemy {
    uint8_t sprite_num;
    Position position;
    Velocity velocity;
    const Node *behavior_tree;
} Enemy;

Enemy initialize_enemy(uint8_t enemy_index, Position position);
void apply_enemy_velocity(Enemy *enemy);
void update_enemy_sprite(Enemy *enemy);

#endif