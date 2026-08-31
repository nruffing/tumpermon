#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>
#include <stdint.h>

#include "behavior/node.h"
#include "kinematics.h"

#define ENEMY_SPRITE_WIDTH_PX 8
#define ENEMY_SPRITE_HEIGHT_PX 8

#define DEFAULT_ENEMY_VELOCITY_X 0
#define DEFAULT_ENEMY_VELOCITY_Y 0

typedef struct Collision {
    bool is_collided;
    uint16_t collided_at_tick;
} Collision;

typedef struct Enemy {
    uint8_t sprite_num;
    Position position;
    Velocity velocity;
    Collision collision;
    const Node *behavior_tree;
} Enemy;

Enemy initialize_enemy(uint8_t enemy_index, Position position);
void apply_enemy_velocity(Enemy *enemy);
void update_enemy_sprite(Enemy *enemy);

#endif