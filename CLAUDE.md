# Tumpermon

A Game Boy homebrew project built with GBDK-2020, started from the `template-minimal`
GBDK example. Built with `lcc`/SDCC, run in Emulicious for dev, flashed to real
cartridges via GBxCart RW + FlashGBX for hardware testing. See `README.md` for
build/run/flash instructions.

## Build

- `make` (macOS/Linux) or `compile.bat` (Windows) from the repo root.
- `GBDK_HOME` defaults to `../../` — this repo is expected to live two directories
  deep inside a GBDK install (e.g. `<gbdk-root>/git/tumpermon`). Override with
  `make GBDK_HOME=/path/to/gbdk` if placed elsewhere.
- Sources are globbed from `src/*.c` and `src/*/*.c` (one subfolder level deep).
- `-Wm-yc` is set in the Makefile to mark the ROM CGB-compatible (required for
  `_cpu == CGB_TYPE` to ever be true at runtime — without it every build silently
  falls back to DMG grayscale, which cost real debugging time early on).
- `make clean` removes build artifacts (also gitignored).

## Architecture

Each module is layered so it only knows about the layer directly below it:

```
main.c (process_frame)
  → joypad.c        raw joypad() → JoypadState (no knowledge of Player)
  → kinematics.c     JoypadState → Velocity  (compute_velocity_from_joypad)
  → player.c         Velocity → Player state (apply_velocity), then → sprite (update_player_sprite)
```

- **`kinematics.{c,h}`** — generic, reusable motion primitives: `Position`,
  `Velocity`, `Direction`, fixed-point conversion, `move_sprite_to_position`.
  Deliberately knows nothing about `Player` — intended to be reused for any
  future entity (enemies, projectiles, etc).
- **`joypad.{c,h}`** — input layer only. `JoypadState`/`InputState` describe raw
  button state (`is_pressed`, `is_just_pressed` — edge-detected via a `static`
  previous-frame value inside `process_joypad`, shared correctly across all
  callers including the splash screen). Deliberately has no `Player` dependency.
- **`player.{c,h}`** — the only module that knows about `Player`. `apply_velocity`
  mutates position/direction only (no hardware calls); `update_player_sprite` is
  a separate step that pushes state to the sprite. Kept separate on purpose —
  when collision detection is added, it belongs *between* these two calls
  (resolve collision against the candidate position before committing state,
  then sync the sprite once) rather than mutate-then-revert.
- **`sprites.{h}`** — VRAM tile index / OAM slot constants (`PLAYER_SPRITE_SLOT`,
  etc.), kept separate from `.c` files so slot assignments are easy to see as
  more sprites are added.
- **`colors.h`** — named CGB RGB555 palette constants (`RGB()`/`RGB8()` from
  `gb/cgb.h`).
- **`splash.c`** — title screen, drawn in APA (`gb/drawing.h`) mode. Leaves APA
  mode (`mode(M_TEXT_OUT)`) and explicitly re-blanks VRAM tile pattern data
  (`utils/util.c`'s `reset_screen`) before gameplay starts — `cls()` alone only
  resets the tile *map*, not the tile pattern data APA drew into, and leaving
  that stale data caused visible corruption (vertical bars) after the splash.
- **`utils/`** — `util.c` (CGB detection, palette reset, VRAM/screen reset),
  `apa_util.c` (APA mode enter/exit, centered-text helpers).

## Conventions / decisions made this session

- **Fixed-point position/velocity**, not floats (no FPU on this CPU).
  `FIXED_POINT_POSITION_LENGTH = 4` (16 units/pixel). Position is `uint16_t`
  (unsigned, screen-space), Velocity is `int16_t` (signed, for direction).
  Convert to real pixels only at render time (`fixed_point_to_pixel`).
- **Screen/velocity coordinate convention**: +x = right, +y = **down** (not up —
  standard top-left-origin screen space, opposite of math/graph convention).
- **`move_sprite()` hardware offset**: GB sprite coordinates are offset by
  `+8` (x) / `+16` (y) from the visible screen position (hardware reserves
  margin for sprites scrolling on/off-screen). Handled once in
  `move_sprite_to_position`, not something callers need to think about.
- **Opposing D-pad directions cancel to 0** rather than one arbitrarily winning
  (`compute_velocity_from_joypad` checks e.g. `left && !right`). Note: this
  couldn't actually be exercised/tested — Emulicious (like real hardware) never
  reports both Left+Right pressed simultaneously, so the cancellation logic is
  correct but currently unverified by observed behavior.
- **Struct composition**: value-embed sub-structs (`Player.position` is a
  `Position`, not a `Position *`) — no heap/malloc on this platform, so a
  pointer member would just add indirection with no ownership benefit.
  Functions still take pointers (`Player *`) when they need to mutate.
- **`static` for file-local helpers** (e.g. `fixed_point_to_pixel`,
  `build_input_state`) — not exposed in the header, since they're
  implementation details of their `.c` file.
- Prefer designated initializers for structs: `Player p = { .position = ... };`.
- `(void)` explicitly on zero-arg function signatures (C, not C++, semantics —
  empty `()` means "unspecified args," not "no args").

## Known gaps / next steps (as of this session)

- **Player sprite is still placeholder art** — a single solid 8x8 tile, not a
  real metasprite. Discussed metasprite structure (`metasprite_t`, frames,
  `move_metasprite_ex`, hardware X/Y flip) and OAM basics in prep for real
  character art; referenced `examples/cross-platform/metasprites` and
  `examples/cross-platform/platformer_template` (whose player character is
  24x32px / 6 hardware sprites in 8x16 mode) as references. Nothing implemented
  yet — current player rendering still goes through single-sprite
  `move_sprite_to_position`, not the metasprite API.
- **`JoypadState` doesn't cover A/B/Select yet** — only up/down/left/right/start.
- **No collision detection** — `apply_velocity` applies velocity unconditionally.
  Planned approach (see Architecture above): candidate position → resolve
  against world (per-axis, not combined, to allow wall-sliding) → commit →
  sync sprite. Not yet implemented.
- **No animation** — direction-facing state exists on `Player`, but nothing
  currently changes sprite tiles based on it.
- **Sub-pixel movement is implemented but movement is simple** — constant
  velocity per frame while a direction is held, no acceleration/friction, no
  diagonal-speed normalization (diagonal movement is faster than cardinal).

## SDCC/GBDK gotchas encountered (useful if debugging build warnings)

- **Angle brackets (`<foo.h>`) vs quotes (`"foo.h"`)**: project-local headers
  need quotes — angle brackets only search GBDK's system include paths, not
  `src/`.
- **`warning 158: overflow in implicit constant conversion`**: SDCC sometimes
  doesn't constant-fold `#define`-based arithmetic (e.g. `GRAPHICS_WIDTH - 1`)
  before checking it against a narrower parameter type, even when the result
  provably fits — a false positive. Fix: explicit cast, e.g.
  `(uint8_t)(GRAPHICS_WIDTH - 1)`.
- **`warning 110`, "so said EVELYN the modified DOG"**: SDCC's (intentionally
  quirky) message for its optimizer eliminating/rewriting a conditional branch
  it proved was constant. Generally harmless.
- **`gprint()` takes non-`const char *`** — GBDK's older APA text API isn't
  const-correct; keep your own strings `const` and cast at the `gprint()` call
  site specifically, rather than dropping `const` everywhere.
