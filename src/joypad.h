#ifndef JOYPAD_H
#define JOYPAD_H

#include <stdbool.h>

typedef struct InputState {
    bool is_pressed;
    bool is_just_pressed;
} InputState;

typedef struct JoypadState {
    InputState up;
    InputState down;
    InputState left;
    InputState right;
    InputState start;
} JoypadState;

JoypadState process_joypad(void);

#endif
