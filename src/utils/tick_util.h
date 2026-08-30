#ifndef TICK_UTIL_H
#define TICK_UTIL_H

#include <stdbool.h>
#include <stdint.h>

// Wraparound-safe comparison of two Context.tick-derived values (e.g.
// InputState.pressed_at_tick, see joypad.h) — hides the `(int16_t)(a - b)`
// subtraction trick behind a name so callers don't need to know about tick
// wraparound themselves. Returns the signed frame distance from `b` to `a`:
// positive means `a` happened after `b` (by that many frames), negative
// means `a` happened before `b`, zero means equal.
//
// Only correct if `a` and `b` are within half the counter's range of each
// other — beyond that, an older and a newer timestamp become
// indistinguishable (the wrapped bit pattern is the same either way), so it
// goes from "safe" to "confidently wrong," not just imprecise. A uint16_t's
// half-range is ~32768 frames (~9 min at ~60fps), comfortably longer than
// any input hold/gap this is meant to compare — that margin is why ticks
// are tracked as uint16_t and not a smaller/cheaper uint8_t (whose
// half-range is only ~128 frames, ~2 sec — too short for e.g. comparing a
// direction held for several seconds against a button pressed later).
int16_t compare_ticks(uint16_t a, uint16_t b);

// Stateless on/off toggle for something that should flip at a fixed
// frequency (e.g. a blinking sprite/cursor), driven purely by `tick` and
// `period_ticks` — no "last toggled at" state for the caller to track.
// Returns true for the first `period_ticks` frames of each `2 *
// period_ticks`-frame cycle, false for the second half, so the caller sees
// an even on/off square wave: on for `period_ticks` frames, off for
// `period_ticks` frames, repeating.
//
// `period_ticks` must be > 0 (a zero period is a divide-by-zero and asserts
// nothing about frequency). Wraps safely with `tick` overflow since the
// modulo is taken before any subtraction-based comparison is involved.
bool tick_toggle_state(uint16_t tick, uint16_t period_ticks);

#endif
