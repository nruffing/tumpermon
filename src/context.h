#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdint.h>

#include "player.h"
#include "kinematics.h"

typedef struct Context {
    // Monotonic frame counter — incremented once per process_frame() call
    // (see main.c), so it's a free logical clock in a vsync-locked game loop
    // with no need for a hardware timer. Use it for relative ordering/
    // duration (e.g. InputState.pressed_at_tick, compared via
    // utils/tick_util.h's compare_ticks), not wall-clock time. See
    // compare_ticks's comment for wraparound behavior/limits — this is a
    // uint16_t for the same reasoning laid out there.
    uint16_t tick;

    Player *player;
    KinematicBehaviorContext *kinematics;
} Context;

#endif
