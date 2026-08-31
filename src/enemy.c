#include "enemy.h"

#include <gb/gb.h>

#include "behavior/enemy_behavior.h"

Enemy initialize_enemy(uint8_t enemy_index, Position position)
{
    uint8_t sprite_num = ENEMY_SPRITE_START_SLOT + enemy_index;
    set_sprite_tile(sprite_num, ENEMY_TILE_START_INDEX);
    // Reuse the player's CGB object palette (loaded once into palette slot 0
    // by update_sprite_pallete(player_palettes) in main.c) instead of loading
    // enemy_palettes (obj/res/enemy.h) into a palette slot of its own —
    // OAMF_CGB_PAL0 is what makes this sprite draw from that same slot,
    // rather than relying on whatever OAM property byte happens to be
    // left over from reset/hardware init.
    set_sprite_prop(sprite_num, OAMF_CGB_PAL0);

    Velocity stationary = { .x = DEFAULT_ENEMY_VELOCITY_X, .y = DEFAULT_ENEMY_VELOCITY_Y };
    Enemy enemy = { .sprite_num = sprite_num,
                    .position = position,
                    .velocity = stationary,
                    .behavior_tree = &basic_enemy_behavior_tree };
    return enemy;
}

void apply_enemy_velocity(Enemy *enemy)
{
    enemy->position.x += enemy->velocity.x;
    enemy->position.y += enemy->velocity.y;
}

void update_enemy_sprite(Enemy *enemy)
{
    move_sprite_to_position(enemy->sprite_num, enemy->position);
}