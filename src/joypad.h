#ifndef JOYPAD_H
#define JOYPAD_H

#include <stdbool.h>
#include <stdint.h>

typedef struct InputState {
    bool is_pressed;
    bool is_just_pressed;
    uint16_t pressed_at_tick;
} InputState;

typedef struct JoypadState {
    InputState up;
    InputState down;
    InputState left;
    InputState right;
    InputState start;
    InputState select;
    InputState a;
    InputState b;
} JoypadState;

JoypadState process_joypad(uint16_t tick);

#endif
