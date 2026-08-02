#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <stdint.h>

#define FIXED_POINT_POSITION_LENGTH 4

typedef struct Position {
    uint16_t x; // fixed-point
    uint16_t y; // fixed-point
} Position;

void move_sprite_to_position(uint8_t sprite_num, Position position);

#endif
