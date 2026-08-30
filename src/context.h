#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>
#include <stdint.h>

#include "enemy.h"
#include "joypad.h"
#include "kinematics.h"
#include "menu.h"
#include "player.h"

#define MAX_ENEMIES 4

typedef struct Context {
    // Monotonic frame counter — incremented once per process_frame() call
    // (see main.c), so it's a free logical clock in a vsync-locked game loop
    // with no need for a hardware timer. Use it for relative ordering/
    // duration (e.g. InputState.pressed_at_tick, compared via
    // utils/tick_util.h's compare_ticks), not wall-clock time. See
    // compare_ticks's comment for wraparound behavior/limits — this is a
    // uint16_t for the same reasoning laid out there.
    uint16_t tick;

    bool is_paused;
    JoypadState joypad_state;

    MenuContext *menu;
    Player *player;
    KinematicBehaviorContext *kinematics;

    Enemy enemies[MAX_ENEMIES];
    uint8_t enemy_count;
} Context;

#endif
