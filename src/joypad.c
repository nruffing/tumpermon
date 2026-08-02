#include "joypad.h"

#include <gb/gb.h>

static InputState build_input_state(uint8_t current, uint8_t previous, uint8_t mask)
{
    InputState state;
    state.is_pressed = current & mask;
    state.is_just_pressed = (current & mask) && !(previous & mask);
    return state;
}

JoypadState process_joypad(void)
{
    static uint8_t joypad_previous = 0;
    uint8_t joypad_current = joypad();

    JoypadState state;
    state.up = build_input_state(joypad_current, joypad_previous, J_UP);
    state.down = build_input_state(joypad_current, joypad_previous, J_DOWN);
    state.left = build_input_state(joypad_current, joypad_previous, J_LEFT);
    state.right = build_input_state(joypad_current, joypad_previous, J_RIGHT);
    state.start = build_input_state(joypad_current, joypad_previous, J_START);

    joypad_previous = joypad_current;

    return state;
}
