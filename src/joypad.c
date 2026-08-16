#include "joypad.h"

#include <gb/gb.h>

// `previous_pressed_at_tick` is this same button's InputState.pressed_at_tick
// from last frame — needed so a held (not just-pressed) button keeps
// reporting the tick of its original press instead of losing it every frame.
static InputState build_input_state(
    uint16_t tick,
    uint8_t current,
    uint8_t previous,
    uint8_t mask,
    uint16_t previous_pressed_at_tick)
{
    InputState state;
    state.is_pressed = current & mask;
    state.is_just_pressed = (current & mask) && !(previous & mask);

    if (state.is_just_pressed) {
        state.pressed_at_tick = tick; // new press — stamp it
    } else if (state.is_pressed) {
        state.pressed_at_tick = previous_pressed_at_tick; // still held — carry it forward
    } else {
        state.pressed_at_tick = 0; // not pressed — no press to report
    }

    return state;
}

JoypadState process_joypad(uint16_t tick)
{
    static uint8_t joypad_previous = 0;
    // Zero-initialized by default (static storage duration) — every button
    // starts as not-pressed/pressed_at_tick 0, same as InputState's meaning
    // for "not pressed".
    static JoypadState state_previous;
    uint8_t joypad_current = joypad();

    JoypadState state;
    state.up = build_input_state(tick, joypad_current, joypad_previous, J_UP, state_previous.up.pressed_at_tick);
    state.down = build_input_state(tick, joypad_current, joypad_previous, J_DOWN, state_previous.down.pressed_at_tick);
    state.left = build_input_state(tick, joypad_current, joypad_previous, J_LEFT, state_previous.left.pressed_at_tick);
    state.right = build_input_state(tick, joypad_current, joypad_previous, J_RIGHT, state_previous.right.pressed_at_tick);
    state.start = build_input_state(tick, joypad_current, joypad_previous, J_START, state_previous.start.pressed_at_tick);
    state.select = build_input_state(tick, joypad_current, joypad_previous, J_SELECT, state_previous.select.pressed_at_tick);
    state.a = build_input_state(tick, joypad_current, joypad_previous, J_A, state_previous.a.pressed_at_tick);
    state.b = build_input_state(tick, joypad_current, joypad_previous, J_B, state_previous.b.pressed_at_tick);

    joypad_previous = joypad_current;
    state_previous = state;

    return state;
}
